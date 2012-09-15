#!/usr/bin/perl

$language = "Bulgarian";
$prefix = "bg";
$script = "cyrillic";

$alphabet = [
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # ghe with upturn (ukrainian)
['�',  ['�','�']],
                   [], # dje (serbian)
                   [], # gje (macedonian)
['�',  ['�','�']],
                   [], # io (mongolian)
                   [], # ukrainian ie
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # dze (macedonian)
['�',  ['�','�']],
                   [], # belarusian-ukrainian i
                   [], # yi (ukrainian)
['�',  ['�','�']],
                   [], # je (macedonian, serbian)
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # lje (macedonian, serbian)
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # nje (macedonian, serbian)
['�',  ['�','�']],
                   [], # barred o (mongolian)
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # tshe (serbian)
                   [], # kje (macedonian)
['�',  ['�','�']],
                   [], # short u (belarusian)
                   [], # straight u (mongolian)
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # dzhe (macedonian, serbian)
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # yeru (belarusian, russian)
['�',  ['�','�']],
                   [],
                   [], # e (belarusian, russian)
['�',  ['�','�']],
['�',  ['�','�']],
                   [],
[],
                   [],
                   []
];

$sortcase = 'Aa';
#$sortcase = 'aA';

$ligatures = [
];

@special = ('?', '!', '.', 'letters', '-', '\'');

do 'make-rules.pl';
