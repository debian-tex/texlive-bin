% This is a change file for DVItype.
%
% 09/27/95 (KA)  Supporting ASCII pTeX
%
@x
@d my_name=='dvitype'
@d banner=='This is DVItype, Version 3.6' {printed when the program starts}
@y
@d my_name=='pdvitype'
@d banner=='This is pDVItype, Version 3.6-p0.4'
  {printed when the program starts}
@z

@x
  parse_arguments;
@y
  init_kanji;
  parse_arguments;
@z

@x
for i:=@'177 to 255 do xchr[i]:='?';
@y
for i:=@'177 to 255 do xchr[i]:=i;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d undefined_commands==250,251,252,253,254,255
@y
@d dir=255 {pTeX direction}
@d undefined_commands==250,251,252,253,254
@z

@x
@d id_byte=2 {identifies the kind of \.{DVI} files described here}
@y
@d id_byte=2 {identifies the kind of \.{DVI} files described here}
@d ptex_id_byte=3 {identifies the kind of pTeX \.{DVI} files described here}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% JFM and pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!width_ptr:0..max_widths; {the number of known character widths}
@y
@!width_ptr:0..max_widths; {the number of known character widths}
@!fnt_jfm_p:array [0..max_fonts] of boolean;
@!jfm_char_code:array [0..max_widths] of integer;
@!jfm_char_type:array [0..max_widths] of integer;
@!jfm_char_font:array [0..max_widths] of integer;
@!jfm_char_type_count:integer;
@!cur_jfm_char_type:integer;

@ @d jfm_hash_size=347

@ @<Types...@>=
@!jfm_char_type_hash_value=0..jfm_hash_size-1;

@ @<Glob...@>=
@!jfm_char_type_hash_table:array[jfm_char_type_hash_value] of integer;
  { first pointer to character information. 0 means null pointer. }
@!jfm_char_type_hash_link:array[0..max_widths] of integer;
  { next pointer to character information. 0 means null pointer. }

@ @<Set init...@>=
for i:=0 to jfm_hash_size-1 do
  jfm_char_type_hash_table[i] := 0;
jfm_char_type[0]:=0;
jfm_char_type_count:=1;

@ Refer char_type table.

@p function get_jfm_char_type(@!fntn:integer;@!jfmc:integer):integer;
  var p:integer; ct:integer;
begin
  p:=jfm_char_type_hash_table[(jfmc+fntn) mod jfm_hash_size];
  ct:=0; { default char_type is 0 }
  while p <> 0 do
    if (jfm_char_code[p] = jfmc) and (jfm_char_font[p] = fntn) then
      begin ct:=jfm_char_type[p]; p:=0; end
    else
      p:=jfm_char_type_hash_link[p];
  get_jfm_char_type:=ct;
end;

@ @<Glob...@>=
@!ptex_p:boolean;
@!dd:eight_bits;
@!ddstack:array [0..stack_size] of eight_bits;
@z

@x
@!lh:integer; {length of the header data, in four-byte words}
@y
@!lh:integer; {length of the header data, in four-byte words}
@!nt:integer;
@!jfm_h:integer;
@z

@x [35] JFM by K.A.
read_tfm_word; lh:=b2*256+b3;
@y
read_tfm_word; lh:=b0*256+b1;
if (lh = 11) or (lh = 9) then
  begin
    print(' (JFM');
    fnt_jfm_p[nf] := true;
    if lh = 9 then print(' tate');
    print(')');
    nt:=b2*256+b3;
    read_tfm_word;
  end
else
  begin
    nt:=0;
    fnt_jfm_p[nf] := false;
  end;
lh:=b2*256+b3;
@z

@x [35] JFM by K.A.
      tfm_design_size:=round(tfm_conv*(((b0*256+b1)*256+b2)*256+b3))
    else goto 9997;
  end;
@y
      tfm_design_size:=round(tfm_conv*(((b0*256+b1)*256+b2)*256+b3))
    else goto 9997;
  end;
for k:=1 to nt do
  begin
    read_tfm_word;
    jfm_char_code[jfm_char_type_count]:=b0*256+b1;
    jfm_char_type[jfm_char_type_count]:=b2*256+b3;
    jfm_char_font[jfm_char_type_count]:=nf;
    jfm_h:= { hash value }
      (jfm_char_code[jfm_char_type_count]+nf) mod jfm_hash_size;
    jfm_char_type_hash_link[jfm_char_type_count]:=
      jfm_char_type_hash_table[jfm_h];
    jfm_char_type_hash_table[jfm_h]:=jfm_char_type_count;
    jfm_char_type_count := jfm_char_type_count + 1
  end;
@z

@x
@p procedure out_text(c:ASCII_code);
begin if text_ptr=line_length-2 then flush_text;
incr(text_ptr); text_buf[text_ptr]:=c;
end;
@y
@p procedure out_text(c:ASCII_code);
begin if text_ptr=line_length-2 then flush_text;
incr(text_ptr);
if c>=177 then text_buf[text_ptr]:=@'77 else text_buf[text_ptr]:=c;
end;

@ @p procedure out_kanji(c:integer);
begin
  if text_ptr>=line_length-3 then flush_text;
  c:=toBUFF(fromDVI(c));
  incr(text_ptr); text_buf[text_ptr]:= Hi(c);
  incr(text_ptr); text_buf[text_ptr]:= Lo(c);
end;

@ output hexdecimal / octal character code.

@d print_hex_digit(#)==if # <= 9 then print((#):1)
                       else case # of
                         10: print(xchr['A']);
                         11: print(xchr['B']);
                         12: print(xchr['C']);
                         13: print(xchr['D']);
                         14: print(xchr['E']);
                         15: print(xchr['F']); { no more cases }
                       end

@p
ifdef('HEX_CHAR_CODE')
procedure print_hex_number(c:integer);
var n:integer;
    b:array[1..8] of integer;
begin
  n:=1;
  while (n<8) and (c<>0) do
    begin b[n]:=c mod 16; c:=c div 16; n:=n+1 end;
  print('"');
  if n=1 then print(xchr['0'])
  else
    begin
      n:=n-1;
      while n>0 do
        begin print_hex_digit(b[n]); n:=n-1 end
    end
end;
endif('HEX_CHAR_CODE')
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
z0: first_par:=z;
@y
z0: first_par:=z;
dir: first_par:=get_byte;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
s:=0; h:=0; v:=0; w:=0; x:=0; y:=0; z:=0; hh:=0; vv:=0;
@y
s:=0; h:=0; v:=0; w:=0; x:=0; y:=0; z:=0; hh:=0; vv:=0; dd:=0;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  four_cases(set1): begin major('set',o-set1+1:1,' ',p:1); goto fin_set;
    end;
  four_cases(put1): begin major('put',o-put1+1:1,' ',p:1); goto fin_set;
@y
  four_cases(set1),four_cases(put1):
    begin
      if fnt_jfm_p[cur_font]=true then
        begin
          cur_jfm_char_type:=get_jfm_char_type(cur_font,p);
          out_kanji(p);
          if o<put1 then
            begin
              minor('set',o-set1+1:1,' ',p:1);
            end
          else begin
            minor('put',o-put1+1:1,' ',p:1);
          end;
ifdef('HEX_CHAR_CODE')
          print('(');
          print_hex_number(p);
          print(')');
endif('HEX_CHAR_CODE')
          print(' type=',cur_jfm_char_type);
          p:=cur_jfm_char_type
        end
      else begin
        if o<put1 then
          begin
            major('set',o-set1+1:1,' ',p:1);
          end
        else begin
          major('put',o-put1+1:1,' ',p:1);
        end;
ifdef('HEX_CHAR_CODE')
        print('(');
        print_hex_number(p);
        print(')');
endif('HEX_CHAR_CODE')
      end;
      goto fin_set;
@z

@x
  @t\4@>@<Cases for commands |nop|, |bop|, \dots, |pop|@>@;
@y
  dir: begin major('dir ',p:1); dd:=p; goto done;
    end;
  @t\4@>@<Cases for commands |nop|, |bop|, \dots, |pop|@>@;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  hstack[s]:=h; vstack[s]:=v; wstack[s]:=w;
@y
  ddstack[s]:=dd;
  hstack[s]:=h; vstack[s]:=v; wstack[s]:=w;
@z

@x
    h:=hstack[s]; v:=vstack[s]; w:=wstack[s];
@y
    dd:=ddstack[s];
    h:=hstack[s]; v:=vstack[s]; w:=wstack[s];
@z

@x
@d out_space(#)==if (p>=font_space[cur_font])or(p<=-4*font_space[cur_font]) then
    begin out_text(" "); hh:=pixel_round(h+p);
    end
  else hh:=hh+pixel_round(p);
@y
@d out_space(#)==if (p>=font_space[cur_font])or(p<=-4*font_space[cur_font]) then
    begin out_text(" ");
      if dd=0 then hh:=pixel_round(h+p) else vv:=pixel_round(v+p);
    end
  else if dd=0 then hh:=hh+pixel_round(p) else vv:=vv+pixel_round(p);
@z

@x
@d out_vmove(#)==if abs(p)>=5*font_space[cur_font] then vv:=pixel_round(v+p)
  else vv:=vv+pixel_round(p);
@y
@d out_vmove(#)==if abs(p)>=5*font_space[cur_font] then
     begin if dd=0 then vv:=pixel_round(v+p) else  hh:=pixel_round(h-p) end
  else if dd=0 then vv:=vv+pixel_round(p) else hh:=hh-pixel_round(p);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Hexadecimal code
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Translate a |set_char|...@>=
begin if (o>" ")and(o<="~") then
  begin out_text(p); minor('setchar',p:1);
  end
else major('setchar',p:1);
@y
@ @<Translate a |set_char|...@>=
begin if (o>" ")and(o<="~") then
  begin out_text(p); minor('setchar',p:1);
  end
else major('setchar',p:1);
ifdef('HEX_CHAR_CODE')
  print(' (');
  print_hex_number(p);
  print(')');
endif('HEX_CHAR_CODE')
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else hh:=hh+char_pixel_width(cur_font)(p);
@y
else if dd=0 then hh:=hh+char_pixel_width(cur_font)(p)
     else vv:=vv+char_pixel_width(cur_font)(p);
@z

@x
hh:=hh+rule_pixels(q); goto move_right
@y
if dd=0 then hh:=hh+rule_pixels(q) else vv:=vv+rule_pixels(q);
goto move_right
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Finish a command that sets |h:=h+q|, then |goto done|@>=
@y
@<Finish a command that sets |h:=h+q|, then |goto done|@>=
if dd=0 then begin
@z

@x
goto done
@y
goto done end
else begin
if (v>0)and(q>0) then if v>infinity-q then
  begin error('arithmetic overflow! parameter changed from ',
@.arithmetic overflow...@>
    q:1,' to ',infinity-q:1);
  q:=infinity-v;
  end;
if (v<0)and(q<0) then if -v>q+infinity then
  begin error('arithmetic overflow! parameter changed from ',
    q:1, ' to ',(-v)-infinity:1);
  q:=(-v)-infinity;
  end;
hhh:=pixel_round(v+q);
if abs(hhh-vv)>max_drift then
  if hhh>vv then vv:=hhh-max_drift
  else vv:=hhh+max_drift;
if showing then if out_mode>mnemonics_only then
  begin print(' v:=',v:1);
  if q>=0 then print('+');
  print(q:1,'=',v+q:1,', vv:=',vv:1);
  end;
v:=v+q;
if abs(v)>max_v_so_far then
  begin if abs(v)>max_v+99 then
    begin error('warning: |v|>',max_v:1,'!');
@.warning: |v|...@>
    max_v:=abs(v);
    end;
  max_v_so_far:=abs(v);
  end;
goto done
end
@z

@x
@ @<Finish a command that sets |v:=v+p|, then |goto done|@>=
@y
@ @<Finish a command that sets |v:=v+p|, then |goto done|@>=
if dd=0 then begin
@z

@x
goto done
@y
goto done end
else begin
p:=-p;
if (h>0)and(p>0) then if h>infinity-p then
  begin error('arithmetic overflow! parameter changed from ',
@.arithmetic overflow...@>
    p:1,' to ',infinity-h:1);
  p:=infinity-h;
  end;
if (h<0)and(p<0) then if -h>p+infinity then
  begin error('arithmetic overflow! parameter changed from ',
    p:1, ' to ',(-h)-infinity:1);
  p:=(-h)-infinity;
  end;
vvv:=pixel_round(h+p);
if abs(vvv-hh)>max_drift then
  if vvv>hh then hh:=vvv-max_drift
  else hh:=vvv+max_drift;
if showing then if out_mode>mnemonics_only then
  begin print(' h:=',h:1);
  if p>=0 then print('+');
  print(p:1,'=',h+p:1,', hh:=',hh:1);
  end;
h:=h+p;
if abs(h)>max_h_so_far then
  begin if abs(h)>max_h+99 then
    begin error('warning: |h|>',max_h:1,'!');
@.warning: |h|...@>
    max_h:=abs(h);
    end;
  max_h_so_far:=abs(h);
  end;
goto done
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  print('level ',ss:1,':(h=',h:1,',v=',v:1,
    ',w=',w:1,',x=',x:1,',y=',y:1,',z=',z:1,
    ',hh=',hh:1,',vv=',vv:1,')');
@y
  begin
    print('level ',ss:1,':(h=',h:1,',v=',v:1,
      ',w=',w:1,',x=',x:1,',y=',y:1,',z=',z:1);
    if ptex_p then print(',d=',dd:1);
    print(',hh=',hh:1,',vv=',vv:1,')');
  end;
@z

@x
if k<>id_byte then bad_dvi('ID byte is ',k:1);
@y
ptex_p:=(k=ptex_id_byte);
if (k<>id_byte) and (not ptex_p) then bad_dvi('ID byte is ',k:1);
@z

@x
print_ln(', maxstackdepth=',max_s:1,', totalpages=',total_pages:1);
@y
print_ln(', maxstackdepth=',max_s:1,', totalpages=',total_pages:1);
if ptex_p then print_ln('pTeX DVI (id=',ptex_id_byte:1,')');
@z

@x
if m<>id_byte then print_ln('identification in byte ',cur_loc-1:1,
@.identification...should be n@>
    ' should be ',id_byte:1,'!');
@y
if (m<>id_byte) and (m<>ptex_id_byte) then
  print_ln('identification in byte ',cur_loc-1:1,
@.identification...should be n@>
    ' should be ',id_byte:1,' or ',ptex_id_byte:1,'!');
@z

@x
const n_options = 8; {Pascal won't count array lengths for us.}
@y
const n_options = 10; {Pascal won't count array lengths for us.}
@z

@x
      usage_help (DVITYPE_HELP, nil);
@y
      usage_help (PDVITYPE_HELP, nil);
@z

@x
    end; {Else it was a flag; |getopt| has already done the assignment.}
@y
    end else if argument_is ('kanji') then begin
      set_prior_file_enc;
      if (not set_enc_string(optarg,optarg)) then begin
        write_ln('Bad kanji encoding "', stringcast(optarg), '".');
      end;

    end; {Else it was a flag; |getopt| has already done the assignment.}
@z

@x
@ An element with all zeros always ends the list.
@y
@ Shift-JIS terminal (the flag is ignored except for WIN32).
@.-sjis-terminal@>

@<Define the option...@> =
long_options[current_option].name := 'sjis-terminal';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := address_of (sjis_terminal);
long_options[current_option].val := 1;
incr (current_option);

@ Decide kanji encode
@.-kanji@>

@<Define the option...@> =
long_options[current_option].name := 'kanji';
long_options[current_option].has_arg := 1;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ An element with all zeros always ends the list.
@z
