#!/usr/bin/env wish

# Copyright 2017-2019 Siep Kroonenberg

# This file is licensed under the GNU General Public License version 2
# or any later version.

package require Tk

# security: disable send
catch {rename send {}}

# make sure TL comes first on process searchpath
if {$::tcl_platform(platform) ne "windows"} {
  set texbin [file dirname [file normalize [info script]]]
  set savedir [pwd]
  cd $texbin
  set texbin [pwd]
  cd $savedir
  # prepend texbin to PATH, unless it is already the _first_
  # path component
  set dirs [split $::env(PATH) ":"]
  if {[lindex $dirs 0] ne $texbin} {
    set ::env(PATH) "${texbin}:$::env(PATH)"
  }
  unset texbin
  unset savedir
  unset dirs
}

# declarations and utilities shared with install-tl-gui.tcl
set ::instroot [exec kpsewhich -var-value=TEXMFROOT]
source [file join $::instroot "tlpkg" "tltcl" "tltcl.tcl"]

# now is a good time to ask tlmgr for the _TL_ name of our platform
set ::our_platform [exec -ignorestderr tlmgr print-platform]

# searchpath and locale:
# windows: most scripts run via [w]runscript, which adjusts the searchpath
# for the current process.
# tlshell.tcl should  be run via a symlink in a directory
# which also contains (a symlink to) kpsewhich.
# This directory will be prepended to the searchpath.
# kpsewhich should disentangle symlinks.

# dis/enable the restore dialog
set do_restore 0

# tlcontrib
set tlcontrib "http://contrib.texlive.info/current"

# dis/enable debug output (only for private development purposes)
set ddebug 0

##### general housekeeping ############################################

# menus: disable tearoff feature
option add *Menu.tearOff 0

proc search_nocase {needle haystack} {
  if {$needle eq ""} {return -1}
  if {$haystack eq ""} {return -1}
  return [string first [string tolower $needle] [string tolower $haystack]]
}

# the stderr and stdout of tlmgr are each read into lists of strings
set err_log {}
set out_log {}

#### debug output, ATM only for private development purposes ####

if $ddebug {set dbg_log {}}

proc do_debug {s} {
  if $::ddebug {
    puts stderr $s
    # On windows, stderr output goes nowhere.
    # Therefore also debug output for the log dialog.
    lappend ::dbg_log $s
    # Track debug output in the log dialog if it is running:
    if [winfo exists .tllg.dbg.tx] {
      .tllg.dbg.tx configure -state normal
      .tllg.dbg.tx insert end "$s\n"
      if {$::tcl_platform(os) ne "Darwin"} {
        .tllg.dbg.tx configure -state disabled
      }
    }
  }
} ; # do_debug

proc maketemp {ext} {
  set fname ""
  foreach i {0 1 2 3 4 5 6 7 8 9} { ; # ten tries
    set fname [file join $::tempsub "[expr {int(10000.0*rand())}]$ext"]
    if {[file exist $fname]} {set fname ""; continue}
    # create empty file. although we just want a name,
    # we must make sure that it can be created.
    set fid [open $fname w]
    close $fid
    if {! [file exists $fname]} {error "Cannot create temporary file"}
    if {$::tcl_platform(platform) eq "unix"} {
      file attributes $fname -permissions 0600
    }
    break
  }
  if {$fname eq ""} {error "Cannot create temporary file"}
  return $fname
} ; # maketemp

set tempsub "" ; # subdir for temp files, set during initialization

### GUI utilities #####################################################

# mouse clicks: deal with MacOS platform differences
if {[tk windowingsystem] eq "aqua"} {
  event add <<RightClick>> <ButtonRelease-2> <Control-ButtonRelease-1>
} else {
  event add <<RightClick>> <ButtonRelease-3>
}

## default_bg color, only used for menus under ::plain_unix
if [catch {ttk::style lookup TFrame -background} ::default_bg] {
  set ::default_bg white
}

# dialog with textbox
proc long_message {str type {p "."}} {
  # alternate messagebox implemented as custom dialog
  # not all message types are supported
  if {$type ne "ok" && $type ne "okcancel" && $type ne "yesnocancel"} {
    err_exit "Unsupported type $type for long_message"
  }
  create_dlg .tlmg $p
  wm title .tlmg ""

  # wallpaper frame; see populate_main
  pack [ttk::frame .tlmg.bg] -fill both -expand 1

  # buttons
  pack [ttk::frame .tlmg.bts] -in .tlmg.bg -side bottom -fill x
  if {$type eq "ok" || $type eq "okcancel"} {
    ttk::button .tlmg.ok -text [__ "Ok"] -command "end_dlg \"ok\" .tlmg"
    ppack .tlmg.ok -in .tlmg.bts -side right
  }
  if {$type eq "yesnocancel"} {
    ttk::button .tlmg.yes -text [__ "Yes"] -command "end_dlg \"yes\" .tlmg"
    ppack .tlmg.yes -in .tlmg.bts -side right
    ttk::button .tlmg.no -text [__ "No"] -command "end_dlg \"no\" .tlmg"
    ppack .tlmg.no -in .tlmg.bts -side right
  }
  if {$type eq "yesnocancel" || $type eq "okcancel"} {
    ttk::button .tlmg.cancel -text [__ "Cancel"] -command \
        "end_dlg \"cancel\" .tlmg"
    ppack .tlmg.cancel -in .tlmg.bts -side right
  }
  if [winfo exists .tlmg.cancel] {
    bind .tlmg <Escape> {.tlmg.cancel invoke}
  } elseif {$type eq "ok"} {
    bind .tlmg <Escape> {.tlmg.ok invoke}
  }

  ppack [ttk::frame .tlmg.tx] -in .tlmg.bg -side top -fill both -expand 1
  pack [ttk::scrollbar .tlmg.tx.scroll -command ".tlmg.tx.txt yview"] \
      -side right -fill y
  ppack [text .tlmg.tx.txt -height 20 -width 60 -bd 2 -relief groove \
      -wrap word -yscrollcommand ".tlmg.tx.scroll set"] -expand 1 -fill both

  .tlmg.tx.txt insert end $str
  .tlmg.tx.txt configure -state disabled

  # default resizable
  place_dlg .tlmg $p
  tkwait window .tlmg
  return $::dialog_ans
} ; # long_message

proc any_message {str type {p "."}} {
  if {[string length $str] < 400} {
    if {$type ne "ok"} {
      return [tk_messageBox -message $str -type $type -parent $p \
                 -icon question]
    } else {
      return [tk_messageBox -message $str -type $type -parent $p]
    }
  } else {
    # just hope that tcl will do the right thing with multibyte characters
    if {[string length $str] > 500000} {
      set str [string range $str 0 500000]
      append str "\n....."
    }
    return [long_message $str $type $p]
  }
} ; # any_message

### enabling and disabling user interaction

proc selective_dis_enable {} {
  # disable actions which make no sense at the time

  # buttons in the middle section
  set pkg_buttons [list .mrk_inst .mrk_upd .mrk_rem .upd_tlmgr .upd_all]
  foreach b [list .mrk_inst .mrk_upd .mrk_rem .upd_tlmgr .upd_all] {
    $b state !disabled
  }
  if $::do_restore {.mrk_rest state !disabled}

  if {!$::have_remote} {
    foreach b [list .mrk_inst .mrk_upd .upd_tlmgr .upd_all] {
      $b state disabled
    }
  } elseif {!$::n_updates} {
    foreach b [list .mrk_upd .upd_tlmgr .upd_all] {
      $b state disabled
    }
  } elseif $::need_update_tlmgr {
    foreach b [list .mrk_inst .mrk_upd] {
      $b state disabled
    }
  } elseif {!$::need_update_tlmgr} {
    .upd_tlmgr state disabled
  }

  # platforms menu item
  if {$::tcl_platform(platform) ne "windows"} {
    if {!$::have_remote || $::need_update_tlmgr}  {
      .mn.opt entryconfigure $::inx_platforms -state disabled
    } else {
      .mn.opt entryconfigure $::inx_platforms -state normal
    }
  }
}; # selective_dis_enable

proc total_dis_enable {y_n} {
  # to be invoked when tlmgr becomes busy or idle,, i.e.
  # if it starts with a tlmgr command through run_cmds
  # or read_line notices the command(s) ha(s|ve) ended.
  # This proc should cover all active interface elements of the main window.
  # But if actions are initiated via a dialog, the main window can be
  # deactivated by a grab and focus on the dialog instead.

  if {!$y_n} {
    . configure -menu .mn_empty
    foreach c [winfo children .] {
      if {$c ne ".showlogs" && [winfo class $c] in \
              [list TButton TCheckbutton TRadiobutton TEntry Treeview]} {
        # this should cover all relevant widgets in the main window
        $c state disabled
      }
    }
    set ::busy [__ "Running"]
  } else {
    . configure -menu .mn
    foreach c [winfo children .] {
      if {[winfo class $c] in \
              [list TButton TCheckbutton TRadiobutton TEntry Treeview]} {
        $c state !disabled
      }
    }
    set ::busy [__ "Idle"]
    selective_dis_enable
  }
} ; # total_dis_enable

##### tl global data ##################################################

set ::last_cmd ""

set ::progname [info script]
regexp {^.*[\\/]([^\\/\.]*)(?:\....)?$} $progname dummy ::progname
set ::procid [pid]

# package repositories
array unset ::repos

# mirrors: dict of dicts of lists of urls per country per continent
set ::mirrors [dict create]

# dict of (local and global) package dicts
set ::pkgs [dict create]

# platforms
set ::platforms [dict create]

set ::have_remote 0 ; # remote packages info not yet loaded
set ::need_update_tlmgr 0
set ::n_updates 0
set ::tlshell_updatable 0

## data to be displayed ##

# sorted display data for packages; package data stored as lists
set ::filtered [dict create]

# selecting packages for display: status and detail
set ::stat_opt "inst"
set ::dtl_opt "all"
# searching packages for display; also search short descriptions?
set ::search_desc 0

##### handling tlmgr via pipe and stderr tempfile #####################

set ::prmpt "tlmgr>"
set ::busy [__ "Running"]

# copy logs to log window yes/no
set ::show_output 0

# about [chan] gets:
# if a second parameter, in this case l, is supplied
# then this variable receives the result, with EOL stripped,
# and the return value is the string length, possibly 0
# EOF is indicated by a return value of -1.

proc read_err_tempfile {} {
  set len 0
  while 1 {
    set len [chan gets $::err l]
    if {$len >= 0} {
      lappend ::err_log $l
    } else {
      break
    }
  }
} ; # read_err_tempfile

proc err_exit {} {
  do_debug "error exit"
  read_err_tempfile
  any_message [join $::err_log "\n"] "ok"
  exit
} ; # err_exit

# Capture stdout of tlmgr into a pipe $::tlshl,
# stderr into a temp file file $::err_file which is set at initialization.

proc start_tlmgr {{args ""}} {
  # Start the TeX Live Manager shell interface.
  # Below, vwait ::done_waiting forces tlshell
  # to process initial tlmgr output before continuing.
  unset -nocomplain ::done_waiting
  do_debug "opening tlmgr"
  if [catch \
          {open "|tlmgr $args --machine-readable shell 2>>$::err_file" w+} \
          ::tlshl] {
    tk_messageBox -message [get_stacktrace]
    exit
  }
  set ::perlpid [pid $::tlshl]
  do_debug "done opening tlmgr"
  set ::err [open $::err_file r]
  chan configure $::tlshl -buffering line -blocking 0
  chan event $::tlshl readable read_line
  vwait ::done_waiting
} ; # start_tlmgr

proc close_tlmgr {} {
  catch {chan close $::tlshl}
  catch {chan close $::err}
  set ::perlpid 0
}; # close_tlmgr

# read a line of tlmgr output
proc read_line {} {
  # a caller of run_cmd needs to explicitly invoke 'vwait ::done_waiting'
  # if it wants to wait for the command to finish
  set l "" ; # will contain the line to be read
  if {([catch {chan gets $::tlshl l} len] || [chan eof $::tlshl])} {
    #do_debug "read_line: failing to read "
    puts stderr "Read failure; tlmgr command was $::last_cmd"
    if {! [catch {chan close $::tlshl}]} {set ::perlpid 0}
    # note. the right way to terminate is terminating the GUI shell.
    # This closes stdin of tlmgr shell.
    err_exit
  } elseif {$len >= 0} {
    # do_debug "read: $l"
    if $::ddebug {puts $::flid $l}
    if {[string first $::prmpt $l] == 0} {
      # prompt line: we are done with the current command
      total_dis_enable 1 ; # this may have to be redone later
      # catch up with stderr
      read_err_tempfile
      if $::show_output {
        do_debug "prompt found, $l"
        log_widget_finish
      }
      # for vwait:
      set ::done_waiting 1
      set ::show_output 0
    } else {
      # regular output
      lappend ::out_log $l
      if $::show_output {
        log_widget_add $l
      }
    }
  }
} ; # read_line

# copy error strings to error page in logs dialog .tllg and send it to top.
# This by itself does not map the logs dialog .tllg

proc show_err_log {} {
  #do_debug "show_err_log"
  .tllg.err.tx configure -state normal
  .tllg.err.tx delete 1.0 end
  if {[llength $::err_log] > 0} {
    foreach l $::err_log {.tllg.err.tx insert end "$l\n"}
    .tllg.err.tx yview moveto 1
    .tllg.logs select .tllg.err
  }
  if {$::tcl_platform(os) ne "Darwin"} {
    # os x: text widget disabled => no selection possible
    .tllg.err.tx configure -state disabled
  }
} ; # show_err_log

proc log_widget_init {} {
  show_logs ; # create the logs dialog
  .tllg.status configure -text [__ "Running"]
  .tllg.close configure -state disabled
}

proc log_widget_add l {
  .tllg.log.tx configure -state normal
  .tllg.log.tx insert end "$l\n"
  if {$::tcl_platform(os) ne "Darwin"} {
    .tllg.log.tx configure -state disabled
  }
}

proc log_widget_finish {} {
  .tllg.log.tx yview moveto 1
  .tllg.logs select .tllg.log
  # error log on top if it contains anything
  show_err_log
  if {$::tcl_platform(os) ne "Darwin"} {
    .tllg.log.tx configure -state disabled
  }
  .tllg.status configure -text [__ "Idle"]
  .tllg.close configure -state !disabled
  bind .tllg <Escape> {.tllg.close invoke}
}

##### running tlmgr commands #####

# run a list of commands
proc run_cmds {cmds {show 0}} {
  set ::show_output $show
  do_debug "run_cmds \"$cmds\""
  if $::ddebug {puts $::flid "\n$cmds"}
  set ::out_log {}
  set ::err_log {}
  if $show {
    show_logs
    .tllg.status configure -text [__ "Running"]
    .tllg.close configure -state disabled
  }
  set l [llength $cmds]
  for {set i 0} {$i<$l} {incr i} {
    set cmd [lindex $cmds $i]
    set ::last_cmd $cmd
    unset -nocomplain ::done_waiting
    # disable widgets for each new command,
    # since read_line will re-enable them
    # when a particular command is finished
    total_dis_enable 0
    chan puts $::tlshl $cmd
    chan flush $::tlshl
    if {$i < [expr {$l-1}]} {vwait ::done_waiting}
  }
} ; # run_cmds

# run a single command
proc run_cmd {cmd {show 0}} {
  run_cmds [list $cmd] $show
} ; # run_cmd

proc run_cmd_waiting {cmd} {
  run_cmd $cmd 0
  vwait ::done_waiting
} ; # run_cmd_waiting

##### Handling package info #####

# what invokes what?
# The main 'globals' are (excepting dicts and arrays):

# ::have_remote is initialized to false. It is set to true by
# get_packages_info_remote, and remains true except temporarily at
# a change of repository.

# The other globals ones are ::n_updates, ::need_update_tlmgr and
# ::tlshell_updatable. These are initially set to 0 and re-calculated
# by update_globals.

# update_globals is invoked by get_packages_info_remote and
# update_local_revnumbers. It enables and disables buttons as appropriate.

# displayed global status info is updated by update_globals.
# update button/menu states are set at initialization and updated
# by update_globals, both via the selective_dis_enable proc

# get_packages_info_local is invoked only once, at initialization.  After
# installations and removals, the collected information is updated by
# update_local_revnumbers.
# Both procs also invoke get_platforms

# get_packages_info_remote will be invoked by collect_filtered if
# ::have_remote is false. Afterwards, ::have_remote will be true, and
# therefore get_packages_info_remote will not be called again.
# get_packages_info_remot also invokes get_platforms
# get_packages_info_remote invokes update_globals.

# update_local_revnumbers will be invoked after any updates. It also
# invokes update_globals.

# collect_filtered does not only filter, but also organize the
# information to be displayed.  If necessary, it invokes
# get_packages_info_remote and always invokes display_packages_info.
# It is invoked at initialization, when filtering options change and
# at the end of install-, remove- and update procs.

# display_packages_info is mostly invoked by collect_filtered, but
# also when the search term or the search option changes.

proc is_updatable {nm} {
  set pk [dict get $::pkgs $nm]
  set lr [dict get $pk localrev]
  set rr [dict get $pk remoterev]
  return [expr {$lr > 0 && $rr > 0 && $rr > $lr}]
}

proc update_globals {} {
  if {! $::have_remote} return
  set ::n_updates 0
  foreach nm [dict keys $::pkgs] {
    if [is_updatable $nm] {incr ::n_updates}
  }
  set ::need_update_tlmgr [is_updatable texlive.infra]
  set ::tlshell_updatable [is_updatable tlshell]

  # also update displayed status info
  if {$::have_remote && $::need_update_tlmgr} {
    .topf.luptodate configure -text [__ "Needs updating"]
  } elseif $::have_remote {
    .topf.luptodate configure -text [__ "Up to date"]
  } else {
    .topf.luptodate configure -text [__ "Unknown"]
  }
  # ... and status of update buttons
  selective_dis_enable
}

# The package display treeview widget in the main window has columns
# for both local and remote revision numbers.
# It gets its data from $::filtered rather than directly from $::pkgs.
# ::pkgs should already be up to date.

# I added a field 'marked' to ::pkgs. It is displayed in the first treeview
# column. This looks better than the treeview tag facilities.
# In ::pkgs, 'marked' is represented as a boolean.
# In ::filtered and .pkglist they are represented as unicode symbols.

# display packages obeying both filter and search string.
# even on a relatively slow system, regenerating .pkglist from ::filtered
# at every keystroke is acceptably responsive.
# with future more advanced search options, this scheme may not suffice.

proc display_packages_info {} {
  # do_debug [get_stacktrace]
  set curr [.pksearch.e get]
  .pkglist delete [.pkglist children {}]
  dict for {nm pk} $::filtered {
    set do_show 0
    if {$curr eq ""} {
      set do_show 1
    } elseif {[search_nocase $curr $nm] >= 0} {
      set do_show 1
    } elseif {$::search_desc && \
          [search_nocase $curr [dict get $::pkgs $nm shortdesc]] >= 0} {
      set do_show 1
    }
    if $do_show {
      .pkglist insert {} end -id $nm -values $pk
    }
  }
} ; # display_packages_info

# (re)create ::filtered dictionary; disregard search string.
# The value associated with each key/package is a list, not another dict.
proc collect_filtered {} {
  do_debug \
      "collect_filtered for $::stat_opt and $::dtl_opt"
  if {$::stat_opt ne "inst" && ! $::have_remote} {
    get_packages_info_remote
  }
  foreach nm [dict keys $::filtered] {
    dict unset ::filtered $nm
  }
  foreach nm [lsort [dict keys $::pkgs]] {
    set pk [dict get $::pkgs $nm]
    set do_show 1
    set mrk [mark_sym [dict get $pk marked]]
    set lr [dict get $pk localrev]
    set rr [dict get $pk remoterev]
    set ct [dict get $pk category]
    if {$::stat_opt eq "inst" && $lr == 0} {
      set do_show 0
    } elseif {$::stat_opt eq "upd" && ($lr == 0 || $rr == 0 || $rr <= $lr)} {
      set do_show 0
    }
    if {! $do_show} continue
    if {$::dtl_opt eq "schm" && $ct ne "Scheme"} {
      set do_show 0
    } elseif {$::dtl_opt eq "coll" && \
        $ct ne "Scheme" && $ct ne "Collection"} {
      set do_show 0
    }
    if {! $do_show} continue

    # collect data to be displayed for $nm
    dict lappend ::filtered $nm $mrk
    dict lappend ::filtered $nm $nm
    set v [dict get $pk localrev]
    set lv [dict get $pk lcatv]
    if {$v eq "0" || $v == 0} {
      set v ""
    } elseif {$lv != 0 && $lv ne ""} {
      set v "$v ($lv)"
    }
    dict lappend ::filtered $nm $v
    set v [dict get $pk remoterev]
    set rv [dict get $pk rcatv]
    if {$v eq "0" || $v == 0} {
      set v ""
    } elseif {$rv != 0 && $rv ne ""} {
      set v "$v ($rv)"
    }
    dict lappend ::filtered $nm $v
    dict lappend ::filtered $nm [dict get $pk shortdesc]
  }
  display_packages_info
} ; # collect_filtered

# derive the set of platforms from the dictionary of packages:
# collect the values $plname from packages 'tex\.$plname'
proc get_platforms {} {
  # guarantee fresh start
  foreach k $::platforms {dict unset ::platforms $k}
  set ::platforms [dict create]
  # glob-style matching: $k should start with "tex."
  foreach k [dict keys $::pkgs "tex.*"] {
    set plname [string range $k 4 end]
    if {$plname eq ""} continue
    set pl [dict create "cur" 0 "fut" 0]
    if {[dict get $::pkgs $k "localrev"] > 0} {
      dict set pl "cur" 1
      dict set pl "fut" 1
    }
    dict set ::platforms $plname $pl
  }
}

# get fresh package list. invoked at program start
# some local packages may not be available online.
# to test, create local dual-platform installation from dvd, try to update
# from more recent linux-only installation

proc get_packages_info_local {} {
  # start from scratch; see also update_local_revnumbers
  foreach nm [dict keys $::pkgs] {
    dict unset ::pkgs $nm
  }
  set ::have_remote 0
  set ::need_update_tlmgr 0
  set ::updatable 0
  set ::tlshell_updatable 0

  run_cmd_waiting \
    "info --only-installed --data name,localrev,cat-version,category,shortdesc"
  set re {^([^,]+),([0-9]+),([^,]*),([^,]*),(.*)$}
  foreach l $::out_log {
    if [regexp $re $l m nm lrev lcatv catg pdescr] {
      # double-quotes in short description: remove outer, unescape inner
      if {[string index $pdescr 0] eq "\""} {
        set pdescr [string range $pdescr 1 end-1]
      }
      set pdescr [string map {\\\" \"} $pdescr]
      dict set ::pkgs $nm \
          [list "marked" 0 "localrev" $lrev "lcatv" $lcatv "remoterev" 0 \
               "rcatv" 0 "category" $catg shortdesc $pdescr]
    }
  }
  get_platforms
} ; # get_packages_info_local

# remote: preserve information on installed packages
proc get_packages_info_remote {} {
  # remove non-local database entries
  foreach k [dict keys $::pkgs] {
    if {! [dict get $::pkgs $k localrev]} {
      dict unset ::pkgs $k
    }
  }
  set ::need_update_tlmgr 0
  set ::updatable 0
  set ::tlshell_updatable 0

  if [catch {run_cmd_waiting \
    "info --data name,localrev,remoterev,cat-version,category,shortdesc"}] {
    do_debug [get_stacktrace]
    tk_messageBox -message [__ "A configured repository is unavailable."]
    return 0
  }
  set re {^([^,]+),([0-9]+),([0-9]+),([^,]*),([^,]*),(.*)$}
  foreach l $::out_log {
    if [regexp $re $l m nm lrev rrev rcatv catg pdescr] {
      # double-quotes in short description: remove outer, unescape inner
      if {[string index $pdescr 0] eq "\""} {
        set pdescr [string range $pdescr 1 end-1]
      }
      set pdescr [string map {\\\" \"} $pdescr]
      if [catch {dict get $::pkgs $nm} pk] {
        # package entry does not yet exist, create it
        dict set ::pkgs $nm [list "marked" 0 "localrev" 0 "lcatv" 0]
      }
      dict set ::pkgs $nm "remoterev" $rrev
      dict set ::pkgs $nm "rcatv" $rcatv
      dict set ::pkgs $nm "category" $catg
      dict set ::pkgs $nm "shortdesc" $pdescr
    }
  }
  get_platforms
  set ::have_remote 1
  .topf.loaded configure -text [__ "Loaded"] -foreground black
  update_globals
  return 1
} ; # get_packages_info_remote

## update ::pkgs after installing packages without going online again.
proc update_local_revnumbers {} {
  do_debug "update_local_revnumbers"
  run_cmd_waiting "info --only-installed --data name,localrev,cat-version"
  set re {^([^,]+),([0-9]+),(.*)$}
  dict for {pk pk_dict} $::pkgs {
    do_debug "zeroing local data for $pk"
    dict set pk_dict "localrev" 0
    dict set pk_dict "lcatv" 0
    dict set ::pkgs $pk $pk_dict
  }
  foreach l $::out_log {
    if [regexp $re $l m pk lr lv] {
      set pk_dict [dict get $::pkgs $pk]
      dict set pk_dict "localrev" $lr
      dict set pk_dict "lcatv" $lv
      dict set ::pkgs $pk $pk_dict
    }
  }
  get_platforms
  update_globals
} ; # update_local_revnumbers

##### Logs notebook ##############################

# if invoked via run_cmds, it tracks progress of (a) tlmgr command(s).
# run_cmds will temporarily disable the close button
# and set .tllg.status to busy via total_dis_enable 0.
# otherwise, it shows the output of the last completed (list of) command(s).

# Note that run_cmds clears ::out_log and ::err_log, but not ::dbg_log.

proc show_logs {} {
  create_dlg .tllg .
  wm title .tllg Logs

  # wallpaper
  pack [ttk::frame .tllg.bg] -fill both -expand 1

  # close button and busy label
  pack [ttk::frame .tllg.bottom] -in .tllg.bg -side bottom -fill x
  ttk::button .tllg.close -text [__ "Close"] -command {end_dlg 0 .tllg}
  ppack .tllg.close -in .tllg.bottom -side right -anchor e
  ppack [ttk::label .tllg.status -anchor w] -in .tllg.bottom -side left
  bind .tllg <Escape> {.tllg.close invoke}

  # notebook pages and scrollbars
  ttk::frame .tllg.log
  pack [ttk::scrollbar .tllg.log.scroll -command ".tllg.log.tx yview"] \
      -side right -fill y
  ppack [text .tllg.log.tx -height 10 -wrap word \
      -yscrollcommand ".tllg.log.scroll set"] \
      -expand 1 -fill both
  .tllg.log.tx configure -state normal
  foreach l $::out_log {
    .tllg.log.tx insert end "$l\n"
  }
  if {$::tcl_platform(os) ne "Darwin"} {.tllg.log.tx configure -state disabled}
  .tllg.log.tx yview moveto 1

  ttk::frame .tllg.err
  pack [ttk::scrollbar .tllg.err.scroll -command ".tllg.err.tx yview"] \
      -side right -fill y
  ppack [text .tllg.err.tx -height 10 -wrap word \
      -yscrollcommand ".tllg.err.scroll set"] \
      -expand 1 -fill both
  .tllg.err.tx configure -state normal
  foreach l $::err_log {
    .tllg.err.tx configure -state normal
    .tllg.err.tx insert end "$l\n"
  }
  if {$::tcl_platform(os) ne "Darwin"} {.tllg.err.tx configure -state disabled}
  .tllg.err.tx yview moveto 1

  if $::ddebug {
    ttk::frame .tllg.dbg
    pack [ttk::scrollbar .tllg.dbg.scroll -command ".tllg.dbg.tx yview"] \
        -side right -fill y
    ppack [text .tllg.dbg.tx -height 10 -bd 2 -relief groove -wrap word \
        -yscrollcommand ".tllg.dbg.scroll set"] \
        -expand 1 -fill both
    .tllg.dbg.tx configure -state normal
    foreach l $::dbg_log {
      .tllg.dbg.tx insert end "$l\n"
    }
    if {$::tcl_platform(os) ne "Darwin"} {
      .tllg.dbg.tx configure -state disabled
    }
    .tllg.dbg.tx yview moveto 1
  }

  # collect pages in notebook widget
  pack [ttk::notebook .tllg.logs] -in .tllg.bg -side top -fill both -expand 1
  .tllg.logs add .tllg.log -text [__ "Output"]
  .tllg.logs add .tllg.err -text [__ "Other"]
  if $::ddebug {
    .tllg.logs add .tllg.dbg -text "Debug"
    raise .tllg.dbg .tllg.logs
  }
  raise .tllg.err .tllg.logs
  raise .tllg.log .tllg.logs

  # default resizable
  place_dlg .tllg .
} ; # show_logs

##### repositories ###############################################

### mirrors

# turn name into a string suitable for a widget name
proc mangle_name {n} {
  set n [string tolower $n]
  set n [string map {" "  "_"} $n]
  return $n
} ; # mangle_name

set mirrors [dict create]
proc read_mirrors {} {
  if [catch {open [file join [exec kpsewhich -var-value SELFAUTOPARENT] \
                   "tlpkg/installer/ctan-mirrors.pl"] r} fm] {
    do_debug "cannot open mirror list"
    return 0
  }
    set re_geo {^\s*'([^']+)' => \{\s*$}
  set re_url {^\s*'(.*)' => ([0-9]+)}
  set re_clo {^\s*\},?\s*$}
  set starting 1
  set lnum 0 ; # line number for error messages
  set ok 1 ; # no errors encountered yet
  set countries {} ; # aggregate list of countries
  set urls {} ; # aggregate list of urls
  set continent ""
  set country ""
  set u ""
  set in_cont 0
  set in_coun 0
  while {! [catch {chan gets $fm} line] && ! [chan eof $fm]} {
    incr lnum
    if $starting {
      if {[string first "\$mirrors =" $line] == 0} {
        set starting 0
        continue
      } else {
        set ok 0
        set msg "Unexpected line '$line' at start"
        break
      }
    }
    # starting is now dealt with.
    if [regexp $re_geo $line dummy c] {
      if {! $in_cont} {
        set in_cont 1
        set continent $c
        set cont_dict [dict create]
        if {$continent in [dict keys $::mirrors]} {
          set ok 0
          set msg "Duplicate continent $c at line $lnum"
          break
        }
      } elseif {! $in_coun} {
        set in_coun 1
        set country $c
        if {$country in $countries} {
          set ok 0
          set msg "Duplicate country $c at line $lnum"
          break
        }
        lappend countries $country
        dict set cont_dict $country {}
      } else {
        set ok 0
        set msg "Unexpected continent- or country line $line at line $lnum"
        break
      }
    } elseif [regexp $re_url $line dummy u n] {
      if {! $in_coun} {
        set ok 0
        set msg "Unexpected url line $line at line $lnum"
        break
      } elseif {$n ne "1"} {
        continue
      }
      append u "systems/texlive/tlnet"
      if {$u in $urls} {
          set ok 0
          set msg "Duplicate url $u at line $lnum"
          break
      }
      dict lappend cont_dict $country $u
      lappend urls $u
      set u ""
    } elseif [regexp $re_clo $line] {
      if $in_coun {
        set in_coun 0
        set country ""
      } elseif $in_cont {
        set in_cont 0
        dict set ::mirrors $continent $cont_dict
        set continent ""
      } else {
        break ; # should close mirror list
      }
    } ; # ignore other lines
  }
  close $fm
  if {! $ok} {do_debug $msg}
} ; # read_mirrors

proc pick_local_repo {} {
  set tail "tlpkg/texlive.tlpdb"
  set nrep [.tlr.cur cget -text]
  if {! [file exists [file join $nrep $tail]]} {
    # not local, try originally configured $::repos(main)
    set nrep $::repos(main)
    if {! [file exists [file join $nrep $tail]]} {
      # again, not local
      set nrep $::env(HOME) ; # HOME also o.k. for windows
    }
  }
  while 1 {
    set nrep [browse4dir $nrep .tlr]
    if {$nrep ne "" && ! [file exists [file join $nrep $tail]]} {
      tk_messageBox -message [__ "%s not a repository" $nrep] -parent .tlr
      continue
    } else {
      break
    }
  }
  if {$nrep ne ""} {
    .tlr.new delete 0 end
    .tlr.new insert end $nrep
  }
} ; # pick_local_repo

proc get_repos_from_tlmgr {} {
  array unset ::repos
  run_cmd_waiting "option repository"
  set rps ""
  foreach l $::out_log {
    if [regexp {repository\t(.*)$} $l dum rps] break
  }
  if {$rps ne ""} {
    set reps [split $rps " "]
    set nr [llength $reps]
    foreach rp $reps {
      # decode spaces and %
      set rp [string map {"%20" " "} $rp]
      set rp [string map {"%25" "%"} $rp]
      if {! [regexp {^(.+)#(.+)$} $rp dum r t]} {
        # no tag; use repository as its own tag
        set r $rp
        set t $rp
      }
      if {$nr == 1} {
        set t "main"
      }
      set ::repos($t) $r
    }
    if {"main" ni [array names ::repos]} {
      array unset ::repos
    }
  }
}; # get_repos_from_tlmgr

proc set_repos_in_tlmgr {} {
  # tlmgr has no command to replace a single repository;
  # we need to compose opt_location ourselves from $::repos.
  set nr [llength [array names ::repos]]
  set opt_repos ""
  set rp ""
  foreach nm [array names ::repos] {
    if {$nr==1} {
      if {$nm ne "main"} {
        err_exit "Internal error"
      } else {
        set rp $::repos(main)
      }
    } else {
      if {$nm eq $::repos($nm)} {
        set rp $nm
      } else {
        set rp $::repos($nm)
        append rp "#$nm"
      }
    }
    # encode % and spaces
    set rp [string map {"%" "%25"} $rp]
    set rp [string map {" " "%20"} $rp]
    append opt_repos " $rp"
  }
  run_cmd_waiting "repository set [string range $opt_repos 1 end]"
}; # set_repos_in_tlmgr

proc print_repos {} {
  set nms [array names ::repos]
  set c [llength $nms]
  if {$c <= 0} {
    return ""
  } elseif {$c == 1} {
    set nm [lindex $nms 0]
    return $::repos($nm)
  } else {
    set s [__ "multiple repositories"]
    set s "($s)"
    foreach nm $nms {
      append s "\n$::repos($nm)"
      if {$nm ne $::repos($nm)} {append s " ($nm)"}
    }
    return $s
  }
} ; # print_repos

proc repos_commit {} {
  set changes 0
  # first remove pinning if appropriate
  # then set repositories
  # then add pinning if appropriate
  if {! [regexp {^\s*$} [.tlr.new get]]} {
    if {$::repos(main) ne [.tlr.new get]} {
      set ::repos(main) [.tlr.new get]
      set changes 1
    }
  }
  set had_contrib 0
  if $::toggle_contrib {
    set changes 1
    foreach nm [array names ::repos] {
      if {$::repos($nm) eq $::tlcontrib} {
        set had_contrib 1
        run_cmd "pinning remove $nm --all" 0
        run_cmd "pinning remove $::repos($nm) --all" 0
        vwait ::done_waiting
        array unset ::repos $nm
      }
    }
    if {! $had_contrib} {
      set ::repos(tlcontrib) $::tlcontrib
    }
  }
  if $changes {
    set_repos_in_tlmgr
    .topf.lrepos configure -text [print_repos]
    close_tlmgr
    start_tlmgr
    # reload remote package information
    if {$::toggle_contrib && ! $had_contrib} {
      run_cmd_waiting "pinning add tlcontrib \"*\""
    }
    set ::have_remote 0
    get_packages_info_remote
    collect_filtered
  }
} ; # repos_commit

# main repository dialog
proc repository_dialog {} {

  # dialog with
  # - popup menu of mirrors (parse tlpkg/installer/ctan-mirrors.pl)
  # - text entry box
  # - directory browser button
  # - ok and cancel buttons

  create_dlg .tlr .
  wm title .tlr [__ "Main Repository"]

  # wallpaper frame; see populate_main
  pack [ttk::frame .tlr.bg] -expand 1 -fill x

  pack [ttk::frame .tlr.info] -in .tlr.bg -expand 1 -fill x
  grid columnconfigure .tlr.info 1 -weight 1
  set row -1

  # current repository
  incr row
  pgrid [ttk::label .tlr.lcur -text [__ "Current:"]] \
      -in .tlr.info -row $row -column 0 -sticky w
  pgrid [ttk::label .tlr.cur -text $::repos(main)] \
      -in .tlr.info -row 0 -column 1 -sticky w
  # proposed new repository
  incr row
  pgrid [ttk::label .tlr.lnew -text [__ "New"]] \
      -in .tlr.info -row $row -column 0 -sticky w
  pgrid [ttk::entry .tlr.new] \
      -in .tlr.info -row $row -column 1 -columnspan 2 -sticky ew

  ### three ways to specify a repository ###
  pack [ttk::frame .tlr.mirbuttons] -in .tlr.bg -fill x
  # 1. default remote repository
  ttk::button .tlr.ctan -text [__ "Any CTAN mirror"] -command {
    .tlr.new delete 0 end
    .tlr.new insert end $::any_mirror
  }
  ppack .tlr.ctan -in .tlr.mirbuttons -side left -fill x
  # 2. specific repository: create a cascading dropdown menu of mirrors
  destroy .tlr.mir.m
  if {[dict size $::mirrors] == 0} read_mirrors
  do_debug "[dict size $::mirrors] mirrors"
  if {[dict size $::mirrors] > 0} {
    ttk::menubutton .tlr.mir -text [__ "Specific mirror..."] \
        -direction below -menu .tlr.mir.m
    ppack .tlr.mir -in .tlr.mirbuttons -side left -fill x
    menu .tlr.mir.m
    dict for {cont d_cont} $::mirrors {
      set c_ed [mangle_name $cont]
      menu .tlr.mir.m.$c_ed
      .tlr.mir.m add cascade -label $cont -menu .tlr.mir.m.$c_ed
      dict for {cntr urls} $d_cont {
        set n_ed [mangle_name $cntr]
        menu .tlr.mir.m.$c_ed.$n_ed
        .tlr.mir.m.$c_ed add cascade -label $cntr -menu .tlr.mir.m.$c_ed.$n_ed
        foreach u $urls {
          .tlr.mir.m.$c_ed.$n_ed add command -label $u \
              -command ".tlr.new delete 0 end; .tlr.new insert end $u"
        }
      }
    }
  }
  # 3. local repository
  ttk::button .tlr.browse -text [__ "Local directory..."] -command {
    .tlr.new delete 0 end; .tlr.new insert end [pick_local_repo]}
  ppack .tlr.browse -in .tlr.mirbuttons -side left -fill x

  ### add/remove tlcontrib ###
  ttk::label .tlr.contribt -text [__ "tlcontrib additional repository"] \
      -font bfont
  pack .tlr.contribt -in .tlr.bg -anchor w -padx 3 -pady [list 10 3]
  pack [ttk::label .tlr.contribl] -in .tlr.bg -anchor w -padx 3 -pady 3
  ttk::checkbutton .tlr.contribb -variable ::toggle_contrib
  pack .tlr.contribb -in .tlr.bg -anchor w -padx 3 -pady [list 3 10]
  set ::toggle_contrib 0
  set has_contrib 0
  foreach nm [array names ::repos] {
    if {$::repos($nm) eq $::tlcontrib} {
      set has_contrib 1
      set contrib_tag $nm
      break
    }
  }
  if $has_contrib {
    .tlr.contribl configure -text [__ "tlcontrib repository is included"]
    .tlr.contribb configure -text [__ "Remove tlcontrib repository"]
  } else {
    .tlr.contribl configure -text [__ "tlcontrib repository is not included"]
    .tlr.contribb configure -text [__ "Add tlcontrib repository"]
  }

  # two ways to close the dialog
  pack [ttk::frame .tlr.closebuttons] -pady [list 10 0] -in .tlr.bg -fill x
  ttk::button .tlr.save -text [__ "Save and Load"] -command {
    #set ::repos(main) [.tlr.new get]
    repos_commit
    end_dlg "" .tlr
  }
  ppack .tlr.save -in .tlr.closebuttons -side right
  ttk::button .tlr.abort -text [__ "Abort"] -command {end_dlg "" .tlr}
  ppack .tlr.abort -in .tlr.closebuttons -side right
  bind .tlr <Escape> {.tlr.abort invoke}

  wm protocol .tlr WM_DELETE_WINDOW {.tlr.abort invoke}
  wm resizable .tlr 1 0
  place_dlg .tlr .
} ; # repository_dialog

### platforms

if {$::tcl_platform(platform) ne "windows"} {

  proc toggle_pl_marked {pl cl} {
    # toggle_pl_marked is triggered by a mouse click only in column #1.
    # 'fut'[ure] should get updated in ::platforms _and_ in .tlpl.pl.

    if {$cl ne "#1"} return
    if {$pl eq $::our_platform} {
      tk_messageBox -message \
          [__ "Cannot remove own platform %s" $::our_platform] \
          -parent .tlpl
      return
    }
    # $mrk: negation of current value of marked for $pl
    set m1 [expr {[dict get $::platforms $pl "fut"] ? 0 : 1}]
    dict set ::platforms $pl "fut" $m1
    set m0 [dict get $::platforms $pl "cur"]
    if {$m0 == $m1} {
      .tlpl.pl set $pl "sup" [mark_sym $m0]
    } else {
      .tlpl.pl set $pl "sup" "[mark_sym $m0] \u21d2 [mark_sym $m1]"
    }
    .tlpl.do configure -state disabled
    dict for {p mrks} $::platforms {
      if {[dict get $mrks "fut"] ne [dict get $mrks "cur"]} {
        .tlpl.do configure -state !disabled
        break
      }
    }
  } ; # toggle_pl_marked

  proc platforms_commit {} {
    set pl_add {}
    set pl_remove {}
    dict for {p pd} $::platforms {
      if {[dict get $pd "cur"] ne [dict get $pd "fut"]} {
        if {[dict get $pd "fut"]} {
          lappend pl_add $p
        } else {
          lappend pl_remove $p
        }
      }
    }
    if {[llength $pl_add] == 0 && [llength $pl_remove] == 0} return
    set cmds {}
    if {[llength $pl_add] > 0} {
      set cmd "platform add "
      append cmd [join $pl_add " "]
      lappend cmds $cmd
    }
    if {[llength $pl_remove] > 0} {
      set cmd "platform remove "
      append cmd [join $pl_remove " "]
      lappend cmds $cmd
    }
    run_cmds $cmds 1
    vwait ::done_waiting
    update_local_revnumbers
    collect_filtered

  } ; # platforms_do

  # the platforms dialog
  proc platforms_select {} {
    create_dlg .tlpl
    wm title .tlpl [__ "Platforms"]
    if $::plain_unix {wm attributes .tlpl -type dialog}

    # wallpaper frame
    pack [ttk::frame .tlpl.bg] -expand 1 -fill both

    # buttons
    pack [ttk::frame .tlpl.but] -in .tlpl.bg -side bottom -fill x
    ttk::button .tlpl.do -text [__ "Apply and close"] -command {
      platforms_commit; end_dlg "" .tlpl
    }
    ttk::button .tlpl.dont -text [__ "Close"] -command \
        {end_dlg "" .tlpl}
    ppack .tlpl.do -in .tlpl.but -side right
    .tlpl.do configure -state disabled
    ppack .tlpl.dont -in .tlpl.but -side right
    bind .tlpl <Escape> {.tlpl.dont invoke}

    # platforms treeview; do we need a scrollbar?
    pack [ttk::frame .tlpl.fpl] -in .tlpl.bg -fill both -expand 1
    ttk::treeview .tlpl.pl -columns {sup plat} -show headings \
        -height [dict size $::platforms] -yscrollcommand {.tlpl.plsb set}
    ppack .tlpl.pl -in .tlpl.fpl -side left -fill both -expand 1
    ttk::scrollbar .tlpl.plsb -orient vertical \
        -command {.tlpl.pl yview}
    ppack .tlpl.plsb -in .tlpl.fpl -side right -fill y -expand 1
    #.tlpl.pl heading sup -text ""
    .tlpl.pl column sup -width [expr {$::cw * 8}]
    .tlpl.pl heading plat -text [__ "platform"] -anchor w
    .tlpl.pl column plat -width [expr {$::cw * 20}]
    dict for {p mks} $::platforms {
      .tlpl.pl insert {} end -id $p -values \
          [list [mark_sym [dict get $mks "cur"]] $p]
    }

    # "#2" refers to the second column, with editable mark symbols
    bind .tlpl.pl <space> {toggle_pl_marked [.tlpl.pl focus] "#1"}
    bind .tlpl.pl <Return> {toggle_pl_marked [.tlpl.pl focus] "#1"}
    # only toggle when column is "sup" i.e. #1
    bind .tlpl.pl <ButtonRelease-1> \
        {toggle_pl_marked \
             [.tlpl.pl identify item %x %y] \
             [.tlpl.pl identify column %x %y]}

    wm resizable .tlpl 0 1
    place_dlg .tlpl .
  } ; # platforms_select

} ; # $::tcl_platform(platform) ne "windows"

##### restore from backup #####

# This is currently rather dangerous.
# ::do_restore is set to 0 or 1 near the top of this source.
# This code, currently disbled, has not been tested in a while.

if $::do_restore {
# dictionary of backups, with mapping to list of available revisions
set bks {}

proc enable_restore {y_n} {
  set st [expr {$y_n ? !disabled : disabled}]
  .tlbk.bklist state $st
  .tlbk.all configure -state $st
  .tlbk.done configure -state $st
} ; # enable_restore

proc finish_restore {} {
  vwait ::done_waiting
  # now log_widget_finish should have run and re-enabled its close button.
  # We won't wait for the log dialog to close, but we will
  # update the packages display in the main window.
  update_local_revnumbers
  collect_filtered
} ; # finish_restore

proc restore_all {} {
  run_cmd "restore --force --all" 1
  finish_restore
} ; # restore_all

proc restore_this {} {
  set id [.tlbk.bklist focus]
  if {$id eq {}} return
  set r [.tlbk.bklist set $id rev]
  if {$r eq {}} return
  set p [.tlbk.bklist set $id pkg]
  if {$p eq {}} {
    set id [.tlbk.bklist parent $id]
    if {$id ne {}} {set p [.tlbk.bklist set $id pkg]}
  }
  if {$p eq {}} return
  set ans [tk_messageBox -message [__ "Restore %s to revision %s?" $p $r] \
               -type okcancel -parent .tlbk]
  if {$ans ne {ok}} return
  run_cmd "restore --force $p $r" 1
  finish_restore
} ; # restore_this

proc bklist_callback_click {x y} {
  set cl [.tlbk.bklist identify column $x $y]
  if {$cl eq "#0"} return
  set id [.tlbk.bklist identify item $x $y]
  .tlbk.bklist focus $id
  restore_this
} ; # bklist_callback_click

proc restore_backups_dialog {} {
  run_cmd_waiting "option autobackup"
  set re {autobackup\t(.*)$}
  set abk 0
  foreach l $::out_log {
    if [regexp $re $l m abk] break
  }
  if {$abk == 0} {
    tk_messageBox -message [__ "No backups configured"]
    return
  }
  run_cmd_waiting "option backupdir"
  set re {backupdir\t(.*)$}
  set bdir ""
  foreach l $::out_log {
    if [regexp $re $l m bdir] break
  }
  if {$bdir eq ""} {
    tk_messageBox -message [__ "No backup directory defined"]
    return
  }
  set bdir [file join [exec kpsewhich -var-value SELFAUTOPARENT] $bdir]
  if {! [file isdirectory $bdir]} {
    tk_messageBox -message [__ "Backup directory %s does not exist" $bdir]
    return
  }
  set pwd0 [pwd]
  cd $bdir
  set backups [lsort [glob *.tar.xz]]
  if {[llength $backups] == 0} {
    tk_messageBox -message [__ "No backups found in $bdir"]
    return
  }
  # dictionary of backups; package => list of available revisions
  set ::bks [dict create]
  set re {^(.*)\.r(\d+)\.tar\.xz$}
  foreach b $backups {
    if [regexp $re $b m pk r] {
      if {$pk in [dict keys $::bks]} {
        dict lappend ::bks $pk $r
      } else {
        dict set ::bks $pk [list $r]
      }
    }
  }
  if {[llength [dict keys $::bks]] == 0} {
    tk_messageBox -message [__ "No packages in backup directory %s" $bdir]
    return
  }
  # invert sort order of revisions for each package
  foreach pk [dict keys $::bks] {
    dict set ::bks $pk [lsort -decreasing [dict get $::bks $pk]]
  }
  toplevel .tlbk -class Dialog
  wm withdraw .tlbk
  wm transient .tlbk .
  wm title .tlbk [__ "Restore from backup"]
  if $::plain_unix {wm attributes .tlbk -type dialog}

  # wallpaper frame; see populate_main
  pack [ttk::frame .tlbk.bg] -expand 1 -fill x

  # the displayed list of backed-up packages
  pack [ttk::frame .tlbk.fbk] -in .tlbk.bg -side top
  # package names not in #0, because we want to trigger actions
  # without automatically opening a parent.
  pack [ttk::treeview .tlbk.bklist -columns {"pkg" "rev"} \
            -show {tree headings} -height 8 -selectmode browse \
            -yscrollcommand {.tlbk.bkvsb set}] -in .tlbk.fbk -side left
  pack [ttk::scrollbar .tlbk.bkvsb -orient vertical -command \
            {.tlbk.bklist yview}] -in .tlbk.fbk -side right -fill y

  .tlbk.bklist heading "pkg" -text [__ "Package"] -anchor w
  .tlbk.bklist heading "rev" -text [__ "Revision"] -anchor w

  .tlbk.bklist column "#0" -width [expr {$::cw * 2}]
  .tlbk.bklist column "pkg" -width [expr {$::cw * 25}]
  .tlbk.bklist column "rev" -width [expr {$::cw * 12}]

  # fill .tlbk.bklist. Use the last available revision.
  # Remember that $::bks is sorted and revisions inversely sorted
  # id must be unique in the entire list: rev does not qualify
  # must as well use $id for package items too
  set id 0
  dict for {pk rlist} $::bks {
    # package
    .tlbk.bklist insert {} end -id [incr id] \
        -values [list $pk [lindex $rlist 0]] -open 0
    set l [llength $rlist]
    # additional revisions
    if {$l > 1} {
      set idparent $id
      for {set i 1} {$i<$l} {incr i} {
        .tlbk.bklist insert $idparent end -id [incr id] \
            -values [list "" [lindex $rlist $i]]
      }
    }
  }

  # since we can only restore one non-last revision at a time, it is better
  # to only show one non-last revision at a time too.
  bind .tlbk.bklist <<TreeviewOpen>> {
    foreach p [.tlbk.bklist children {}] {
      if {$p ne [.tlbk.bklist focus]} {
        .tlbk.bklist item $p -open 0
      }
    }
  }
  # restoring a single package
  bind .tlbk.bklist <<RightClick>> {bklist_callback_click %x %y}
  bind .tlbk.bklist <space> restore_this

  # frame with buttons
  pack [ttk::frame .tlbk.fbut] -in .tlbk.bg -side bottom -fill x
  ppack [ttk::button .tlbk.all -text [__ "Restore all"] -command restore_all] \
        -in .tlbk.fbut -side right
  ppack [ttk::button .tlbk.done -text [__ "Close"] -command {
    end_dlg "" .tlbk}] -in .tlbk.fbut -side right

  place_dlg .tlbk .
  wm resizable .tlbk 0 0
  tkwait .tlbk
} ; # restore_backups_dialog

} ; # if $::do_restore

##### Main window and supporting procs and callbacks ##################

##### package-related #####

proc update_tlmgr {} {
  if {! $::need_update_tlmgr} {
    tk_messageBox -message [__ "Nothing to do!"]
    return
  }
  if {$::tcl_platform(platform) eq "windows"} {
    set ans [tk_messageBox -type okcancel -icon info -message \
        [string cat [__ "If update fails, try on a command-line:"] \
           "\ntlmgr update --self\n" \
             [__ "Use an administrative command prompt for an admin install."]]]
    if {$ans eq "cancel"} return
  }
  run_cmd "update --self" 1
  vwait ::done_waiting
  # tlmgr restarts itself automatically
  update_local_revnumbers
  collect_filtered
} ; # update_tlmgr

proc update_all {} {
  set updated_tlmgr 0
  if $::need_update_tlmgr {
    run_cmd "update --self" 1
    vwait ::done_waiting
    # tlmgr restarts itself automatically
    update_local_revnumbers
    set updated_tlmgr 1
  }
  # tlmgr restarts itself automatically
  #  tk_messageBox -message [__ "Update self first!"]
  #  return
  if {! $::n_updates && !$updated_tlmgr} {
    tk_messageBox -message [__ "Nothing to do!"]
    return
  } elseif $::n_updates {
    run_cmd "update --all" 1
    vwait ::done_waiting
    update_local_revnumbers
  }
  collect_filtered
} ; # update_all

### doing something with some packages

proc pkglist_from_option {opt {pk ""}} {
  if {$opt eq "marked"} {
    set pks {}
    dict for {p props} $::pkgs {
      if [dict get $props "marked"] {lappend pks $p}
    }
  } elseif {$opt eq "focus"} {
    set p [.pkglist focus]
    if {$p ne {}} {lappend pks $p}
  } elseif {$opt eq "name"} {
    lappend pks $pk
  }
  return $pks
} ; # pkglist_from_option

proc install_pkgs {sel_opt {pk ""}} {
  set pks [pkglist_from_option $sel_opt $pk]
  # check whether packages are installed
  set pre_installed {}
  set todo {}
  foreach p $pks {
    if {[dict get $::pkgs $p localrev] > 0} {
      lappend pre_installed $p
    } else {
      lappend todo $p
    }
  }
  if {[llength $todo] == 0} {
    tk_messageBox -message [__ "Nothing to do!"] -type ok -icon info
    return
  }
  run_cmd_waiting "install --dry-run $todo"
  # check whether dependencies are going to be installed
  set r {^(\S+)\s+i\s}
  set deps {}
  foreach l $::out_log {
    if {[regexp $r $l d p] && $p ni $pks} {
      lappend deps $p
    }
  }
  if {[llength $deps] > 0} {
    set ans \
        [any_message \
             [__ "Also installing dependencies\n\n%s" $deps] \
             "okcancel"]
    if {$ans eq "cancel"} return
  }
  run_cmd "install $todo" 1
  vwait ::done_waiting
  if {[llength $pre_installed] > 0} {
    lappend ::err_log [__ "Already installed: %s" $pre_installed]
    show_err_log
  }
  update_local_revnumbers
  if {$sel_opt eq "marked"} {mark_all 0}
  collect_filtered
} ; # install_pkgs

proc update_pkgs {sel_opt {pk ""}} {
  set pks [pkglist_from_option $sel_opt $pk]
  # check whether packages are installed
  set not_inst {}
  set uptodate {}
  set todo {}
  foreach p $pks {
    set lv [dict get $::pkgs $p localrev]
    if {[dict get $::pkgs $p localrev] == 0} {
      lappend not_inst $p
    } else {
      set rv [dict get $::pkgs $p remoterev]
      if {$lv >= $rv} {
        lappend uptodate $p
      } else {
        lappend todo $p
      }
    }
  }
  if {[llength $todo] == 0} {
    tk_messageBox -message [__ "Nothing to do!"] -type ok -icon info
    return
  }
  run_cmd_waiting "update --dry-run $todo"
  # check whether dependencies are going to be updated
  set r {^(\S+)\s+u\s}
  set deps {}
  foreach l $::out_log {
    if {[regexp $r $l d p] && $p ni $pks} {
      lappend deps $p
    }
  }
  if {[llength $deps] > 0} {
    set ans [any_message [__ "Also updating dependencies\n\n%s?" $deps] \
       "yesnocancel"]
    switch $ans {
      "cancel" return
      "yes" {run_cmd "update $todo" 1}
      "no" {
        set deps {}
        run_cmd_waiting "update --dry-run --no-depends $todo"
        foreach l $::out_log {
          if {[regexp $r $l u p] && $p ni $pks} {
            lappend deps $p
          }
        }
        if {[llength $deps] > 0} {
          set ans [any_message \
              [__ "Updating some dependencies %s anyway. Continue?" $deps] \
                       "okcancel"]
          if {$ans eq "cancel"} return
        }
        run_cmd "update --no-depends $todo" 1
      }
    }
  } else {
    run_cmd "update $todo" 1
  }
  vwait ::done_waiting
  if {[llength $not_inst] > 0} {
    lappend ::err_log [__ "Skipped because not installed: %s" $not_inst]
  }
  if {[llength $uptodate] > 0} {
    lappend ::err_log [__ "Skipped because already up to date: %s" $uptodate]
  }
  if {[llength $not_inst] > 0 || [llength $uptodate] > 0} {
    show_err_log
  }
  update_local_revnumbers
  if {$sel_opt eq "marked"} {mark_all 0}
  collect_filtered
} ; # update_pkgs

proc remove_pkgs {sel_opt {pk ""}} {
  set pks [pkglist_from_option $sel_opt $pk]
  # check whether packages are installed
  set not_inst {}
  set todo {}
  foreach p $pks {
    if {[dict get $::pkgs $p localrev] > 0} {
      lappend todo $p
    } else {
      lappend not_inst $p
    }
  }
  run_cmd_waiting "remove --dry-run $todo"
  # check whether dependencies are going to be removed
  set r {^(\S+)\s+d\s}
  set deps {}
  foreach l $::out_log {
    if {[regexp $r $l d p] && $p ni $pks} {
      lappend deps $p
    }
  }
  if {[llength $todo] == 0} {
    tk_messageBox -message [__ "Nothing to do!"] -type ok -icon info
    return
  }
  if {[llength $deps] > 0} {
    set ans [any_message [__ "Also remove dependencies\n\n%s?" $deps] \
                "yesnocancel"]
    switch $ans {
      "cancel" return
      "yes" {run_cmd "remove $todo" 1}
      "no" {
        set deps {}
        run_cmd_waiting "remove --dry-run --no-depends $todo"
        foreach l $::out_log {
          if {[regexp $r $l d p] && $p ni $pks} {
            lappend deps $p
          }
        }
        if {[llength $deps] > 0} {
          set ans [any_message \
              [__ "Removing some dependencies %s anyway. Continue?" $deps] \
                       "okcancel"]
          if {$ans eq "cancel"} return
        }
        run_cmd "remove --no-depends $todo" 1
      }
    }
  } else {
    run_cmd "remove $todo" 1
  }
  vwait ::done_waiting
 if {[llength $not_inst] > 0} {
    lappend ::err_log "Skipped because not installed: $not_inst"
    show_err_log
  }
  update_local_revnumbers
  if {$sel_opt eq "marked"} {mark_all 0}
  collect_filtered
} ; # remove_pkgs

# restoring packages is a rather different story, controlled by the
# contents of the backup directory. see further up.

##### varous callbacks #####

proc restart_self {} {
  do_debug "trying to restart"
  if {$::progname eq ""} {
    tk_messageBox -message "progname not found; not restarting"
    return
  }
  close_tlmgr
  exec $::progname &
  # on windows, it may take several seconds before
  # the old tlshell disappears.
  # oh well, windows is still windows....
  destroy .
} ; # restart_self

proc toggle_marked_pkg {itm cl} {
  # toggle_marked_pkg is triggered by a mouse click only in column #1.
  # 'marked' should get updated in ::pkgs, ::filtered and in .pkglist.

  if {$cl ne "#1"} return
  # $mrk: negation of current value of marked for $itm
  set mrk [expr {[dict get $::pkgs $itm "marked"] ? 0 : 1}]
  dict set ::pkgs $itm "marked" $mrk
  set m [mark_sym $mrk]
  dict set ::filtered $itm [lreplace [dict get $::filtered $itm] 0 0 $m]
  .pkglist set $itm mk $m
} ; # toggle_marked_pkg

proc mark_all {mrk} {
  foreach nm [dict keys $::pkgs] {
    dict set ::pkgs $nm "marked" $mrk
  }
  set m [mark_sym $mrk]
  foreach nm [dict keys $::filtered] {
    dict set ::filtered $nm [lreplace [dict get $::filtered $nm] 0 0 $m]
  }
  foreach nm [.pkglist children {}] {
    .pkglist set $nm mk $m
  }
  # alternatively: regenerate ::filtered and .pkglist from ::pkgs
} ; # mark_all

##### package popup #####

proc do_package_popup_menu {x y X Y} {
  # as focused item, the identity of the clicked item will be
  # globally available:
  .pkglist focus [.pkglist identify item $x $y]
  # recreate menu with only applicable items
  set lr [dict get $::pkgs [.pkglist focus] "localrev"]
  set rr [dict get $::pkgs [.pkglist focus] "remoterev"]
  .pkg_popup delete 0 end

  .pkg_popup add command -label [__ "Info"] -command {
    run_cmd "info [.pkglist focus]" 1; vwait ::done_waiting
  }
  if {$::have_remote && ! $::need_update_tlmgr && $rr > 0 && $lr == 0} {
    .pkg_popup add command -label [__ "Install"] -command {
      install_pkgs "focus"
    }
  }
  if {$::have_remote && ! $::need_update_tlmgr && $lr > 0 && $rr > $lr} {
    .pkg_popup add command -label [__ "Update"] -command {
      update_pkgs "focus"
    }
  }
  if {$lr > 0} {
    .pkg_popup add command -label [__ "Remove"] -command {
      remove_pkgs "focus"
    }
  }
  .pkg_popup post [expr {$X - 2}] [expr {$Y - 2}]
  focus .pkg_popup
} ; # do_package_popup_menu

proc set_paper {p} {
  run_cmd "paper paper $p" 1
}

proc set_language_no_restart {l} {
  set ok 1
  if [catch {exec kpsewhich -var-value "TEXMFCONFIG"} d] {set ok 0}
  if $ok {
    set d [file join $d "tlmgr"]
    if [catch {file mkdir $d}] {set ok 0}
  }
  set fn [file join $d "config"]
  set oldlines [list]
  if {$ok && ! [catch {open $fn r} fid]} {
    set cnt 0
    while 1 {
      if [catch {chan gets $fid} ll] break
      if [chan eof $fid] break
      incr cnt
      if {! [regexp {^\s*gui-lang} $ll]} {
          lappend oldlines $ll
      }
      if {$cnt>20} break
    }
    catch {chan close $fid}
  }
  lappend oldlines "gui-lang = $l"
  if {$ok && ! [catch {open $fn w} fid]} {
    foreach ll $oldlines {
      if [catch {puts $fid $ll}] {
        set ok 0
        break
      }
    }
    catch {chan close $fid}
  }
  return $ok
} ; # set_language_no_restart

proc set_language {l} {
  if [set_language_no_restart $l] {
    restart_self
  } else {
    tk_messageBox -message [__ "Cannot set default GUI language"] -icon error
  }
}

##### running external commands #####

# For capturing an external command, we need a separate output channel,
# but we reuse ::out_log.
# stderr is bundled with stdout so ::err_log should stay empty.
proc read_capt {} {
  set l "" ; # will contain the line to be read
  if {([catch {chan gets $::capt l} len] || [chan eof $::capt])} {
    catch {chan close $::capt}
    log_widget_finish
    set ::done_waiting 1
  } elseif {$len >= 0} {
    lappend ::out_log $l
    log_widget_add $l
  }
}; # read_capt

proc run_external {cmd mess} {
  set ::out_log {}
  set ::err_log {}
  lappend ::out_log $mess
  unset -nocomplain ::done_waiting
  # dont understand why, on windows, start_tlmgr does not trigger
  # a console window but this proc does
  if [catch {open "|$cmd 2>&1" "r"} ::capt] {
    tk_messageBox -message "Failure to launch $cmd"
  }
  chan configure $::capt -buffering line -blocking 0
  chan event $::capt readable read_capt
  log_widget_init
}

proc show_help {} {
  set ::env(NOPERLDOC) 1
  long_message [exec tlmgr --help] ok
}

proc run_entry {} {
  # TODO: some validation of $cmd
  set cmd [.tlcust.e get]
  if {$cmd eq ""} return
  run_cmd $cmd 1
  end_dlg "" .tlcust
}

## arbitrary commands: no way to know what data have to be updated
#proc custom_command {} {
#  create_dlg .tlcust .
#  wm title .tlcust [__ "Custom command"]
#  pack [ttk::frame .tlcust.bg] -expand 1 -fill x
#
#  ppack [ttk::entry .tlcust.e] \
#      -in .tlcust.bg -side left -fill x -expand 1
#  ppack [ttk::button .tlcust.b -text [__ "Go"] -command run_entry] \
#      -in .tlcust.bg -side left
#  bind .tlcust.e <Return> run_entry
#  bind .tlcust <Escape> {end_dlg "" .tlcust}
#  wm .tlcust resizable 1 0
#  place_dlg .tlcust .
#}

##### main window #####

proc populate_main {} {

  wm withdraw .

  wm title . "TeX Live Shell"

  # width of '0', as a rough estimate of average character width
  set ::cw [font measure TkTextFont "0"]

  # dummy empty menu to replace the real menu .mn in disabled states.
  # the "File" cascade should ensure that the dummy menu
  # occupies the same vertical space as the real menu.
  menu .mn_empty
  .mn_empty add cascade -label [__ "File"] -menu .mn_empty.file -underline 0
  if $::plain_unix {
    .mn_empty configure -borderwidth 1
    .mn_empty configure -background $::default_bg
  menu .mn_empty.file
  }
  # real menu
  menu .mn
  . configure -menu .mn
  if $::plain_unix {
    .mn configure -borderwidth 1
    .mn configure -background $::default_bg

    # plain_unix: avoid a RenderBadPicture error on quitting.
    # 'send' changes the shutdown sequence,
    # which avoids triggering the bug.
    # 'tk appname <something>' restores 'send' and avoids the bug
    bind . <Destroy> {
      catch {tk appname appname}
    }
  }

  .mn add cascade -label [__ "File"] -menu .mn.file -underline 0
  menu .mn.file
  .mn.file add command -label [__ "Load default repository"] \
      -command {get_packages_info_remote; collect_filtered}
  .mn.file add command -command {destroy .} -label [__ "Exit"] -underline 1

  # inx: keeping count to record indices where needed,
  # i.e. when an entry needs to be referenced.
  # not all submenus need this.

  .mn add cascade -label [__ "Actions"] -menu .mn.act -underline 0
  menu .mn.act
  incr inx
  .mn.act add command -label [__ "Regenerate filename database"] -command \
      {run_external "mktexlsr" [__ "Regenerating filename database..."]}
  .mn.act add command -label [__ "Regenerate formats"] -command \
      {run_external "fmtutil-sys --all" [__ "Rebuilding formats..."]}
  .mn.act add command -label [__ "Regenerate fontmaps"] -command \
      {run_external "updmap-sys" [__ "Rebuilding fontmap files..."]}
  #.mn.act add command -label [__ "Custom command"] -command custom_command

  .mn add cascade -label [__ "Options"] -menu .mn.opt -underline 0

  menu .mn.opt
  set inx -1
  incr inx
  .mn.opt add command -label "[__ "Repositories"] ..." \
      -command repository_dialog

  incr inx
  .mn.opt add cascade -label [__ "Paper ..."] -menu .mn.opt.paper
  incr inx
  menu .mn.opt.paper
  foreach p [list A4 letter] {
    .mn.opt.paper add command -label $p -command "set_paper $p"
  }

  if {[llength $::langs] > 1} {
    incr inx
    .mn.opt add cascade -label [__ "GUI language (restarts tlshell)"] \
        -menu .mn.opt.lang
    menu .mn.opt.lang
    foreach l $::langs {
      .mn.opt.lang add command -label $l -command "set_language $l"
    }
  }

  if {$::tcl_platform(platform) ne "windows"} {
    incr inx
    set ::inx_platforms $inx
    .mn.opt add command -label "[__ "Platforms"] ..." -command platforms_select
  }

  .mn add cascade -label [__ "Help"] -menu .mn.help -underline 0
  menu .mn.help
  .mn.help add command -label [__ "About"] -command {
    tk_messageBox -message [string cat "\u00a9 2017-2019 Siep Kroonenberg

" [__ "GUI interface for TeX Live Manager\nImplemented in Tcl/Tk"]]}
  .mn.help add command -label [__ "tlmgr help"] -command show_help

  # wallpaper frame
  # it is possible to set a background color for a toplevel, but on
  # MacOS I did not find a way to determine the right $::default_bg
  # color. Instead, all toplevels are given a wallpaper ttk::frame
  # with the default ttk::frame color, which seems to work
  # everywhere.
  pack [ttk::frame .bg] -expand 1 -fill both
  .bg configure -padding 5

  # bottom of main window
  pack [ttk::frame .endbuttons] -in .bg -side bottom -fill x
  ttk::label .busy -textvariable ::busy -font TkHeadingFont -anchor w
  ppack .busy -in .endbuttons -side left
  ppack [ttk::button .q -text [__ Quit] -command {destroy .}] \
      -in .endbuttons -side right
  ppack [ttk::button .r -text [__ "Restart self"] -command restart_self] \
      -in .endbuttons -side right
  ppack [ttk::button .t -text [__ "Restart tlmgr"] \
             -command {close_tlmgr; start_tlmgr}] \
      -in .endbuttons -side right
  ttk::button .showlogs -text [__ "Show logs"] -command show_logs
  ppack .showlogs -in .endbuttons -side right

  # various info
  # frame .topf -background white -borderwidth 2 -relief sunken
  ppack [ttk::frame .topf] -in .bg -side top -anchor w -fill x
  pack [ttk::separator .sp -orient horizontal] \
      -in .bg -side top -fill x -pady 6

  ttk::label .topf.llrepo -text [__ "Default repositories"] -anchor w
  pgrid .topf.llrepo -row 0 -column 0 -sticky nw
  ttk::label .topf.lrepos -text "" -justify left -anchor w
  pgrid .topf.lrepos -row 0 -column 1 -sticky nw
  ttk::label .topf.loaded -text [__ "Not loaded"] -foreground red -anchor w
  pgrid .topf.loaded -row 1 -column 1 -sticky w

  ttk::label .topf.lluptodate -text [__ "TL Manager up to date?"] -anchor w
  pgrid .topf.lluptodate -row 2 -column 0 -sticky w
  ttk::label .topf.luptodate -text [__ "Unknown"] -anchor w
  pgrid .topf.luptodate -row 2 -column 1 -sticky w

  ttk::label .topf.llcmd -anchor w -text [__ "Last tlmgr command:"] -anchor w \
      -wraplength [expr {60*$::cw}] -justify left
  pgrid .topf.llcmd -row 3 -column 0 -sticky w
  ttk::label .topf.lcmd -anchor w -textvariable ::last_cmd -anchor w
  pgrid .topf.lcmd -row 3 -column 1 -sticky w

  # package list
  ttk::label .lpack -text [__ "Package list"] -font TkHeadingFont -anchor w
  pack .lpack -in .bg -side top -padx 3 -pady [list 15 3] -fill x

  # controlling package list
  ttk::frame .pkfilter
  pack .pkfilter -in .bg -side top -fill x
  grid columnconfigure .pkfilter 3 -weight 1
  # column #3 is empty, but that is allright
  # filter on status: inst, all, upd
  ttk::label .pkfilter.lstat -font TkHeadingFont -text [__ "Status"]
  ttk::radiobutton .pkfilter.inst -text [__ "Installed"] -value inst \
      -variable ::stat_opt -command collect_filtered
  ttk::radiobutton .pkfilter.alls -text [__ "All"] -value all \
      -variable ::stat_opt -command collect_filtered
  ttk::radiobutton .pkfilter.upd -text [__ "Updatable"] -value upd \
      -variable ::stat_opt -command collect_filtered
  grid .pkfilter.lstat -column 0 -row 0 -sticky w -padx {3 50}
  pgrid .pkfilter.inst -column 0 -row 1 -sticky w
  pgrid .pkfilter.alls -column 0 -row 2 -sticky w
  pgrid .pkfilter.upd -column 0 -row 3 -sticky w

  # filter on detail level: all, coll, schm
  ttk::label .pkfilter.ldtl -font TkHeadingFont -text [__ "Detail >> Global"]
  ttk::radiobutton .pkfilter.alld -text [__ All] -value all \
      -variable ::dtl_opt -command collect_filtered
  ttk::radiobutton .pkfilter.coll -text [__ "Collections and schemes"] \
      -value coll -variable ::dtl_opt -command collect_filtered
  ttk::radiobutton .pkfilter.schm -text [__ "Only schemes"] -value schm \
      -variable ::dtl_opt -command collect_filtered
  pgrid .pkfilter.ldtl -column 1 -row 0 -sticky w
  pgrid .pkfilter.alld -column 1 -row 1 -sticky w
  pgrid .pkfilter.coll -column 1 -row 2 -sticky w
  pgrid .pkfilter.schm -column 1 -row 3 -sticky w

  # marks
  grid [ttk::button .mrk_all -text [__ "Mark all"] -command {mark_all 1}] \
      -in .pkfilter -column 2 -row 1 -sticky w -padx {50 3} -pady 3
  grid [ttk::button .mrk_none -text [__ "Mark none"] -command {mark_all 0}] \
      -in .pkfilter -column 2 -row 2 -sticky w -padx {50 3} -pady 3

  # actions
  set rw -1
  incr rw
  ttk::button .mrk_inst -text [__ "Install marked"] -command {
      install_pkgs "marked"}
  pgrid .mrk_inst -in .pkfilter -column 4 -row $rw -sticky ew
  incr rw
  ttk::button .mrk_upd -text [__ "Update marked"] -command {
    update_pkgs "marked"}
  pgrid .mrk_upd -in .pkfilter -column 4 -row $rw -sticky ew
  incr rw
  ttk::button .mrk_rem -text [__ "Remove marked"] -command {
    remove_pkgs "marked"}
  pgrid .mrk_rem -in .pkfilter -column 4 -row $rw -sticky ew
  if $::do_restore {
    incr rw
    ttk::button .mrk_rest -text "[__ "Restore from backup"] ..." -command \
        restore_backups_dialog
    pgrid .mrk_rest -in .pkfilter -column 4 -row $rw -sticky ew
  }
  incr rw
  ttk::button .upd_tlmgr -text [__ "Update tlmgr"] -command update_tlmgr
  pgrid .upd_tlmgr -in .pkfilter -column 4 -row $rw -sticky ew
  incr rw
  ttk::button .upd_all -text [__ "Update all"] -command update_all
  pgrid .upd_all -in .pkfilter -column 4 -row $rw -sticky ew

  # search interface; no new row
  grid [ttk::frame .pksearch] -in .pkfilter -row $rw \
      -column 0 -columnspan 4 -sticky w
  ppack [ttk::label .pksearch.l \
             -text [__ "Search"]] -side left
  pack [ttk::entry .pksearch.e -width 30] -side left -padx {3 0} -pady 3
  ppack [ttk::radiobutton .pksearch.n -variable ::search_desc \
            -value 0 -text [__ "By name"]] -side left
  ppack [ttk::radiobutton .pksearch.d -variable ::search_desc \
             -value 1 -text [__ "By name and description"]] -side left
  bind .pksearch.e <KeyRelease> display_packages_info
  bind .pksearch.n <ButtonRelease> {set ::search_desc 0; display_packages_info}
  bind .pksearch.d <ButtonRelease> {set ::search_desc 1; display_packages_info}

  # packages list itself
  pack [ttk::frame .fpkg] -in .bg -side top -fill both -expand 1
  ttk::treeview .pkglist -columns \
      {mk name localrev remoterev shortdesc} \
      -show headings -height 8 -selectmode extended \
      -yscrollcommand {.pkvsb set}
  .pkglist heading mk -text "" -anchor w
  .pkglist heading name -text [__ "Name"] -anchor w
  .pkglist heading localrev -text [__ "Local rev. (ver.)"] -anchor w
  .pkglist heading remoterev -text [__ "Remote rev. (ver.)"] -anchor w
  .pkglist heading shortdesc -text [__ "Description"] -anchor w
  .pkglist column mk -width [expr {$::cw * 3}] -stretch 0
  .pkglist column name -width [expr {$::cw * 25}] -stretch 1
  .pkglist column localrev -width [expr {$::cw * 18}] -stretch 0
  .pkglist column remoterev -width [expr {$::cw * 18}] -stretch 0
  .pkglist column shortdesc -width [expr {$::cw * 50}] -stretch 1

  ttk::scrollbar .pkvsb -orient vertical -command {.pkglist yview}
  ppack .pkglist -in .fpkg -side left -expand 1 -fill both
  ppack .pkvsb -in .fpkg -side left -fill y

  # "#1" refers to the first column (with mark symbols)
  bind .pkglist <space> {toggle_marked_pkg [.pkglist focus] "#1"}
  bind .pkglist <Return> {toggle_marked_pkg [.pkglist focus] "#1"}
  # only toggle when column is "mk" i.e. #1
  bind .pkglist <ButtonRelease-1> {toggle_marked_pkg [
      .pkglist identify item %x %y] [.pkglist identify column %x %y]}

  menu .pkg_popup ; # entries added on-the-fly
  bind .pkglist <<RightClick>> {do_package_popup_menu %x %y %X %Y}
  if $::plain_unix {
    bind .pkg_popup <Leave> {.pkg_popup unpost}
  }

  wm protocol . WM_DELETE_WINDOW {.q invoke}
  wm resizable . 1 1
  wm state . normal
}

##### initialize ######################################################

proc initialize {} {
  # seed random numbers
  expr {srand([clock seconds])}

  # directory for temp files
  set attemptdirs {}
  foreach tmp {TMPDIR TEMP TMP} {
    if {$tmp in [array names ::env]} {
      lappend attemptdirs $::env($tmp)
    }
  }
  if {$::tcl_platform(platform) eq "unix"} {
    lappend attemptdirs "/tmp"
  }
  lappend attemptdirs [pwd]
  set ::tempsub ""
  foreach tmp $attemptdirs {
    if {$::tcl_platform(platform) eq "windows"} {
      regsub -all {\\} $tmp {/} tmp
    }
    if {[file isdirectory $tmp]} {
      # no real point in randomizing directory name itself
      if {$::tcl_platform(platform) eq "unix"} {
        set ::tempsub [file join $tmp $::env(USER)]
      } else {
        set ::tempsub [file join $tmp $::env(USERNAME)]
      }
      append ::tempsub "-tlshell"
      if {! [catch {file mkdir $::tempsub}]} {break} ;# success
    }
  }

  if {$::tempsub eq "" || [file isdirectory $::tempsub] == 0} {
    error "Cannot create directory for temporary files"
  }
  # temp file for stderr
  set ::err_file [maketemp ".err_tlshl"]

  # logfile
  if $::ddebug {
    set fname [file join $::tempsub \
      [clock format [clock seconds] -format {%H:%M}]]
    set ::flid [open $fname w]
  }

  # languages
  set ::langs [list "en"]
  foreach l [glob -nocomplain -directory \
                 [file join $::instroot "tlpkg" "translations"] *.po] {
    lappend ::langs [string range [file tail $l] 0 end-3]
  }

  # store language in tlmgr configuration
  set_language_no_restart $::lang

  # in case we are going to do something with json:
  # add json subdirectory to auto_path, but at low priority
  # since the tcl/tk installation may already have a better implementation.
  # Trust kpsewhich to find out own directory and bypass symlinks.
  #set tlsdir [file dirname [exec kpsewhich -format texmfscripts tlshell.tcl]]
  #lappend ::auto_path [file join $tlsdir "json"]

  populate_main

  start_tlmgr
  get_repos_from_tlmgr
  .topf.lrepos configure -text [print_repos]
  get_packages_info_local
  collect_filtered ; # invokes display_packages_info
  selective_dis_enable
}; # initialize

initialize
