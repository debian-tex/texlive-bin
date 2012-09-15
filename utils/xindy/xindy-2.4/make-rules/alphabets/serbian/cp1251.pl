#!/usr/bin/perl

$language = "Serbian";
$prefix = "sr";
$script = "cyrillic";

$alphabet = [
['�',  ['�','�'] ],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # ghe with upturn (ukrainian)
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # gje (macedonian)
['�',  ['�','�'] ],
                   [], # io (mongolian)
                   [], # ukrainian ie
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # dze (macedonian)
['�',  ['�','�'] ],
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
['�',  ['�','�'] ],
                   [], # barred o (mongolian)
['�',  ['�','�']],
['�',  ['�','�'] ],
['�',  ['�','�']],
['�',  ['�','�']],
['�',  ['�','�']],
                   [], # kje (macedonian)
['�',  ['�','�'] ],
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
                   [], # soft sign (ukrainian)
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
