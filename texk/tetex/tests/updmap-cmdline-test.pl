#!/usr/bin/env perl
# $Id: updmap-cmdline-test.pl 26402 2012-05-15 11:09:26Z peter $
# Public domain.  Originally written 2011, Karl Berry.
# Check that updmap can parse various cmdline invocations.

# srcdir = tetex (in the source tree)
BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `cd \`dirname $0\`/.. && pwd`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main {
  my $ret = &test_run ("$srcdir/updmap.pl", "--version");

  # we aren't going to fix this in the current updmap.pl.
                       # "foo", "--enable Map"
                       # "-n", "--quiet",
  my $bad = $ret != 0;
  return $bad;
}
