/*************************************************************************
** SpecialManager.cpp                                                   **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2014 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

#include <config.h>
#include <iomanip>
#include <sstream>
#include "SpecialActions.h"
#include "SpecialHandler.h"
#include "SpecialManager.h"
#include "PsSpecialHandler.h"
#include "macros.h"

using namespace std;


SpecialManager::~SpecialManager () {
	unregisterHandlers();
}


SpecialManager& SpecialManager::instance() {
	static SpecialManager sm;
	return sm;
}


/** Remove all registered handlers. */
void SpecialManager::unregisterHandlers () {
	FORALL(_pool, vector<SpecialHandler*>::iterator, it)
		delete *it;
	_pool.clear();
	_handlers.clear();
	_endPageListeners.clear();
	_positionListeners.clear();
}


/** Registers a single special handler. This method doesn't check if a
 *  handler of the same class is already registered.
 *  @param[in] handler pointer to handler to be registered */
void SpecialManager::registerHandler (SpecialHandler *handler) {
	if (handler) {
		// get array of prefixes this handler is responsible for
		_pool.push_back(handler);
		for (const char **p=handler->prefixes(); *p; ++p)
			_handlers[*p] = handler;
		if (DVIPreprocessingListener *listener = dynamic_cast<DVIPreprocessingListener*>(handler))
			_preprocListeners.push_back(listener);
		if (DVIEndPageListener *listener = dynamic_cast<DVIEndPageListener*>(handler))
			_endPageListeners.push_back(listener);
		if (DVIPositionListener *listener = dynamic_cast<DVIPositionListener*>(handler))
			_positionListeners.push_back(listener);
	}
}


/** Registers several special handlers at once.
 *  If ignorelist == 0, all given handlers are registered. To exclude selected sets of
 *  specials, the corresponding names can be given separated by non alpha-numeric characters,
 *  e.g. "color, ps, em" or "color: ps em" etc.
 *  @param[in] handlers pointer to zero-terminated array of handlers to be registered
 *  @param[in] ignorelist list of special names to be ignored */
void SpecialManager::registerHandlers (SpecialHandler **handlers, const char *ignorelist) {
	if (handlers) {
		string ign = ignorelist ? ignorelist : "";
		FORALL(ign, string::iterator, it)
			if (!isalnum(*it))
				*it = '%';
		ign = "%"+ign+"%";

		for (; *handlers; handlers++) {
			if (!(*handlers)->name() || ign.find("%"+string((*handlers)->name())+"%") == string::npos)
				registerHandler(*handlers);
			else
				delete *handlers;
		}
	}
}


/** Looks for an appropriate handler for a given special prefix.
 *  @param[in] prefix the special prefix, e.g. "color" or "em"
 *  @return in case of success: pointer to handler, 0 otherwise */
SpecialHandler* SpecialManager::findHandler (const string &prefix) const {
	ConstIterator it = _handlers.find(prefix);
	if (it != _handlers.end())
		return it->second;
	return 0;
}


static string extract_prefix (istream &is) {
	int c;
	string prefix;
	while (isalnum(c=is.get()))
		prefix += c;
	if (ispunct(c)) // also add seperation character to identifying prefix
		prefix += c;
	if (prefix == "ps:" && is.peek() == ':')
		prefix += is.get();
	return prefix;
}


void SpecialManager::preprocess (const string &special, SpecialActions *actions) const {
	istringstream iss(special);
	string prefix = extract_prefix(iss);
	if (SpecialHandler *handler = findHandler(prefix))
		handler->preprocess(prefix.c_str(), iss, actions);
}


/** Executes a special command.
 *  @param[in] special the special expression
 *  @param[in] dvi2bp factor to convert DVI units to PS points
 *  @param[in] actions actions the special handlers can perform
 *  @return true if the special could be processed successfully
 *  @throw SpecialException in case of errors during special processing */
bool SpecialManager::process (const string &special, double dvi2bp, SpecialActions *actions) const {
	istringstream iss(special);
	string prefix = extract_prefix(iss);
	bool success=false;
	if (SpecialHandler *handler = findHandler(prefix)) {
		handler->setDviScaleFactor(dvi2bp);
		success = handler->process(prefix.c_str(), iss, actions);
	}
	return success;
}


void SpecialManager::notifyPreprocessingFinished () const {
	FORALL(_preprocListeners, vector<DVIPreprocessingListener*>::const_iterator, it)
		(*it)->dviPreprocessingFinished();
}


void SpecialManager::notifyEndPage (unsigned pageno) const {
	FORALL(_endPageListeners, vector<DVIEndPageListener*>::const_iterator, it)
		(*it)->dviEndPage(pageno);
}


void SpecialManager::notifyPositionChange (double x, double y) const {
	FORALL(_positionListeners, vector<DVIPositionListener*>::const_iterator, it)
		(*it)->dviMovedTo(x, y);
}


void SpecialManager::writeHandlerInfo (ostream &os) const {
	ios::fmtflags osflags(os.flags());
	typedef map<string, SpecialHandler*> SortMap;
	SortMap m;
	FORALL(_handlers, ConstIterator, it)
		if (it->second->name())
			m[it->second->name()] = it->second;
	FORALL(m, SortMap::iterator, it) {
		os << setw(10) << left << it->second->name() << ' ';
		if (it->second->info())
			os << it->second->info();
		os << endl;
	}
	os.flags(osflags);  // restore format flags
}

