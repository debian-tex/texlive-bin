#!/usr/bin/perl

$language = "Macedonian";
$prefix = "mk";
$script = "cyrillic";

$alphabet = [
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # ghe with upturn (ukrainian)
['�',  ['�','�']],
                   [], # dje (serbian)
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # io (mongolian)
                   [], # ukrainian ie
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # belarusian-ukrainian i
                   [], # yi (ukrainian)
                   [], # short i (many)
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # barred o (mongolian)
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # tshe (serbian)
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # short u (belarusian)
                   [], # straight u (mongolian)
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # shcha (many)
                   [], # hard sign (bulgarian, russian)
                   [], # yeru (belarusian, russian)
                   [], # soft sign (many)
                   [],
                   [], # e (belarusian, russian)
                   [], # yu (many)
                   [], # ya (many)
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
