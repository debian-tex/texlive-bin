/*************************************************************************
** XMLNode.cpp                                                          **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <map>
#include <list>
#include <sstream>
#include "XMLNode.hpp"
#include "XMLString.hpp"

using namespace std;


XMLElementNode::XMLElementNode (const string &n) : _name(n) {
}


XMLElementNode::XMLElementNode (const XMLElementNode &node)
	: _name(node._name), _attributes(node._attributes)
{
	for (const auto &child : node._children)
		_children.emplace_back(unique_ptr<XMLNode>(child->clone()));
}


void XMLElementNode::clear () {
	_attributes.clear();
	_children.clear();
}


void XMLElementNode::addAttribute (const string &name, const string &value) {
	_attributes[name] = value;
}


void XMLElementNode::addAttribute (const string &name, double value) {
	_attributes[name] = XMLString(value);
}


void XMLElementNode::append (XMLNode *child) {
	if (!child)
		return;
	XMLTextNode *textNode1 = dynamic_cast<XMLTextNode*>(child);
	if (!textNode1 || _children.empty())
		_children.emplace_back(unique_ptr<XMLNode>(child));
	else {
		if (XMLTextNode *textNode2 = dynamic_cast<XMLTextNode*>(_children.back().get()))
			textNode2->append(textNode1);  // merge two consecutive text nodes
		else
			_children.emplace_back(unique_ptr<XMLNode>(child));
	}
}


void XMLElementNode::append (const string &str) {
	if (_children.empty() || !dynamic_cast<XMLTextNode*>(_children.back().get()))
		_children.emplace_back(unique_ptr<XMLNode>(new XMLTextNode(str)));
	else
		static_cast<XMLTextNode*>(_children.back().get())->append(str);
}


void XMLElementNode::prepend (XMLNode *child) {
	if (!child)
		return;
	XMLTextNode *textNode1 = dynamic_cast<XMLTextNode*>(child);
	unique_ptr<XMLNode> child_uptr(child);
	if (textNode1 && !_children.empty()) {
		if (XMLTextNode *textNode2 = dynamic_cast<XMLTextNode*>(_children.front().get())) {
			textNode2->prepend(textNode1);  // merge two consecutive text nodes
			return;
		}
	}
	_children.emplace_front(std::move(child_uptr));
}


/** Inserts a new child node before a given child node already present. The latter
 *  will be the following sibling of the node to be inserted. If there's no such
 *  node present, nothing is inserted.
 *  @param[in] child node to be inserted
 *  @param[in] sibling following sibling of 'child'
 *  @return true on success */
bool XMLElementNode::insertBefore (XMLNode *child, XMLNode *sibling) {
	ChildList::iterator it = _children.begin();
	while (it != _children.end() && it->get() != sibling)
		++it;
	if (it == _children.end())
		return false;
	_children.emplace(it, unique_ptr<XMLNode>(child));
	return true;
}


/** Inserts a new child node after a given child node already present. The latter
 *  will be the preceding sibling of the node to be inserted. If there's no such
 *  node present, nothing is inserted.
 *  @param[in] child node to be inserted
 *  @param[in] sibling preceding sibling of 'child'
 *  @return true on success */
bool XMLElementNode::insertAfter (XMLNode *child, XMLNode *sibling) {
	ChildList::iterator it = _children.begin();
	while (it != _children.end() && it->get() != sibling)
		++it;
	if (it == _children.end())
		return false;
	_children.emplace(++it, unique_ptr<XMLNode>(child));
	return true;
}


/** Removes a given child from the element. */
void XMLElementNode::remove (const XMLNode *child) {
	_children.remove_if([=](const unique_ptr<XMLNode> &ptr) {
		return ptr.get() == child;
	});
}


/** Gets all descendant elements with a given name and given attribute.
 *  @param[in] name name of elements to find
 *  @param[in] attrName name of attribute to find
 *  @param[out] descendants all elements found
 *  @return true if at least one element was found  */
bool XMLElementNode::getDescendants (const char *name, const char *attrName, vector<XMLElementNode*> &descendants) const {
	for (auto &child : _children) {
		if (XMLElementNode *elem = dynamic_cast<XMLElementNode*>(child.get())) {
			if ((!name || elem->getName() == name) && (!attrName || elem->hasAttribute(attrName)))
				descendants.push_back(elem);
			elem->getDescendants(name, attrName, descendants);
		}
	}
	return !descendants.empty();
}


/** Returns the first descendant element that matches the given properties in depth first order.
 *  @param[in] name element name; if 0, all elements are taken into account
 *  @param[in] attrName if not 0, only elements with an attribute of this name are considered
 *  @param[in] attrValue if not 0, only elements with attribute attrName="attrValue" are considered
 *  @return pointer to the found element or 0 */
XMLElementNode* XMLElementNode::getFirstDescendant (const char *name, const char *attrName, const char *attrValue) const {
	for (auto &child : _children) {
		if (XMLElementNode *elem = dynamic_cast<XMLElementNode*>(child.get())) {
			if (!name || elem->getName() == name) {
				const char *value;
				if (!attrName || (((value = elem->getAttributeValue(attrName)) != 0) && (!attrValue || string(value) == attrValue)))
					return elem;
			}
			if (XMLElementNode *descendant = elem->getFirstDescendant(name, attrName, attrValue))
				return descendant;
		}
	}
	return 0;
}


ostream& XMLElementNode::write (ostream &os) const {
	os << '<' << _name;
	for (const auto &attrib : _attributes)
		os << ' ' << attrib.first << "='" << attrib.second << '\'';
	if (_children.empty())
		os << "/>";
	else {
		os << '>';
		// Insert newlines around children except text nodes. According to the
		// SVG specification, pure whitespace nodes are ignored by the SVG renderer.
		if (!dynamic_cast<XMLTextNode*>(_children.front().get()))
			os << '\n';
		for (auto it=_children.begin(); it != _children.end(); ++it) {
			(*it)->write(os);
			if (!dynamic_cast<XMLTextNode*>(it->get())) {
				auto next=it;
				if (++next == _children.end() || !dynamic_cast<XMLTextNode*>(next->get()))
					os << '\n';
			}
		}
		os << "</" << _name << '>';
	}
	return os;
}


/** Returns true if this element has an attribute of given name. */
bool XMLElementNode::hasAttribute (const string &name) const {
	return _attributes.find(name) != _attributes.end();
}


/** Returns the value of an attribute.
 *  @param[in] name name of attribute
 *  @return attribute value or 0 if attribute doesn't exist */
const char* XMLElementNode::getAttributeValue(const std::string& name) const {
	AttribMap::const_iterator it = _attributes.find(name);
	if (it != _attributes.end())
		return it->second.c_str();
	return 0;
}


//////////////////////

void XMLTextNode::append (XMLNode *node) {
	if (!node)
		return;
	if (XMLTextNode *tn = dynamic_cast<XMLTextNode*>(node))
		append(tn);
	else {
		// append text representation of the node
		ostringstream oss;
		node->write(oss);
		append(XMLString(oss.str()));
		delete node;
	}
}


void XMLTextNode::append (XMLTextNode *node) {
	if (node)
		_text += node->_text;
	delete node;
}


void XMLTextNode::append (const string &str) {
	_text += str;
}


void XMLTextNode::prepend (XMLNode *node) {
	if (XMLTextNode *tn = dynamic_cast<XMLTextNode*>(node))
		_text = tn->_text + _text;
	else
		delete node;
}


//////////////////////


ostream& XMLCDataNode::write (ostream &os) const {
	if (!_data.empty())
		os << "<![CDATA[\n" << _data << "]]>";
	return os;
}


void XMLCDataNode::append (string &&str) {
	if (_data.empty())
		_data = move(str);
	else
		_data += str;
}