#!/usr/bin/perl

$language = "Ukrainian";
$prefix = "uk";
$script = "cyrillic";

$alphabet = [
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # dje (serbian)
                   [], # gje (macedonian)
['�',  ['�','�'],['�','�']],
                   [], # io (mongolian)
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # dze (macedonian)
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
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
['�',  ['�','�']],
                   [], # soft sign (many)
                   [],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
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
