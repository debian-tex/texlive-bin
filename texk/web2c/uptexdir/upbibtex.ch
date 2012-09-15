@x
@d banner=='This is pBibTeX, Version 0.99d-j0.33'
@y
@d banner=='This is upBibTeX, Version 0.99d-j0.33-u1.10'
@z

@x
for i:=@'200 to @'237 do xchr[i]:=chr(i-@'200);
for i:=@'240 to 255 do xchr[i]:=chr(i);
@y
for i:=@'200 to 255 do xchr[i]:=chr(i);
@z

@x
for i:=@'200 to @'237 do xord[i]:= i-@'200;
for i:=@'240 to 255 do xord[i]:=i;
@y
for i:=@'200 to 255 do xord[i]:=i;
@z

@x
@d zen_pun_first = 161          {Zenkaku punctuation first byte; in EUC}
@d zen_space = 161              {Zenkaku space first, second byte; in EUC}
@d zen_kuten = 162              {Zenkaku kuten second byte; in EUC}
@d zen_ten = 163                {Zenkaku ten second byte; in EUC}
@d zen_comma = 164              {Zenkaku comman second byte; in EUC}
@d zen_period = 165             {Zenkaku period second byte; in EUC}
@d zen_question = 169           {Zenkaku question mark second byte; in EUC}
@d zen_exclamation = 170        {Zenkaku exclamation mark second byte; in EUC}
@y
@d e_ss3 = @"8F                 {single shift three in EUC}
@d e_pun_first = @"A1           {Zenkaku punctuation first byte; in EUC}
@d e_space = @"A1A1             {Zenkaku space; in EUC}
@d e_toten = @"A1A2             {Zenkaku kuten; in EUC}
@d e_kuten = @"A1A3             {Zenkaku toten; in EUC}
@d e_comma = @"A1A4             {Zenkaku comman; in EUC}
@d e_period = @"A1A5            {Zenkaku period; in EUC}
@d e_question = @"A1A9          {Zenkaku question mark; in EUC}
@d e_exclamation =@"A1AA        {Zenkaku exclamation mark; in EUC}
@d u_pun_first1 = @"E3          {Zenkaku punctuation first byte(1); in UTF-8}
@d u_pun_first2 = @"EF          {Zenkaku punctuation first byte(2); in UTF-8}
@d u_space = @"3000             {Zenkaku space; in UCS}
@d u_toten = @"3001             {Zenkaku toten; in UCS}
@d u_kuten = @"3002             {Zenkaku kuten; in UCS}
@d u_comma = @"FF0C             {Zenkaku comman; in UCS}
@d u_period = @"FF0E            {Zenkaku period; in UCS}
@d u_question = @"FF1F          {Zenkaku question mark; in UCS}
@d u_exclamation = @"FF01       {Zenkaku exclamation mark; in UCS}
@d u_double_question = @"2047   {Zenkaku double question mark; in UCS}
@d u_double_exclam   = @"203C   {Zenkaku double exclamation mark; in UCS}
@d u_question_exclam = @"2048   {Zenkaku question exclamation mark; in UCS}
@d u_exclam_question = @"2049   {Zenkaku exclamation question mark; in UCS}
@z

@x
for i:=@'200 to @'237 do lex_class[i] := illegal;
for i:=@'240 to 255 do lex_class[i] := alpha;
lex_class[@'33]:=alpha;
@y
@z

@x
for i:=@'200 to @'237 do id_class[i] := illegal_id_char;
@y
@z

@x
for i:=@'240 to 254 do char_width[i]:=514;
@y
@z

@x procedure get_the_top_level_aux_file_name
label aux_found,@!aux_not_found;
@y
label aux_found,@!aux_not_found;
var i:0..last_text_char;    {this is the first one declared}
@z
@x
  @<Process a possible command line@>
@y
  @<Process a possible command line@>
  @<Initialize variables depending on Kanji code@>
@z

@x
    zen_ten,
    zen_period,
    zen_question,
    zen_exclamation:
        if( str_pool[sp_ptr-1] = zen_pun_first ) then
            repush_string
        else
            @<Add the |period| (it's necessary) and push@>;
    othercases
        @<Add the |period| (it's necessary) and push@>
@y
    othercases
        begin
        if (is_internalEUC) then
            begin
            case (fromBUFF(str_pool, sp_ptr+1, sp_ptr-1)) of
                e_kuten,
                e_period,
                e_question,
                e_exclamation:
                    if (str_pool[sp_ptr-2]<>e_ss3) then
                       repush_string
                    else
                       @<Add the |period| (it's necessary) and push@>;
                othercases
                    @<Add the |period| (it's necessary) and push@>;
            endcases;
            end;
        if (is_internalUPTEX) then
            begin
            case (fromBUFF(str_pool, sp_ptr+1, sp_ptr-2)) of
                u_kuten,
                u_period,
                u_question,
                u_exclamation,
                u_double_question,
                u_double_exclam,
                u_question_exclam,
                u_exclam_question:
                    repush_string;
                othercases
                    @<Add the |period| (it's necessary) and push@>;
            endcases;
            end;
        end;
@z

@x
    if(str_pool[str_start[pop_lit1]]>127) then { a KANJI char is 2byte long }
@y
    if(str_pool[str_start[pop_lit1]]>127) then { a KANJI char is |2..4|byte long }
@z

@x
    if( (ex_buf[ex_buf_ptr-1]=zen_comma) or (ex_buf[ex_buf_ptr-1]=zen_kuten) )
    then ex_buf_ptr := ex_buf_ptr - 2
    else ex_buf_ptr := ex_buf_ptr - 4;
@y
    if (is_internalEUC) then
      if((fromBUFF(ex_buf, ex_buf_length, ex_buf_ptr-2) = e_comma) or
         (fromBUFF(ex_buf, ex_buf_length, ex_buf_ptr-2) = e_toten))
      then ex_buf_ptr := ex_buf_ptr - 2
      else ex_buf_ptr := ex_buf_ptr - 4;
    if (is_internalUPTEX) then
      if((fromBUFF(ex_buf, ex_buf_length, ex_buf_ptr-3) = u_comma) or
         (fromBUFF(ex_buf, ex_buf_length, ex_buf_ptr-3) = u_toten))
      then ex_buf_ptr := ex_buf_ptr - 3
      else ex_buf_ptr := ex_buf_ptr - 4;
@z

@x
     zen_pun_first:
        begin
          if((ex_buf[ex_buf_ptr+1]=zen_comma) or
             (ex_buf[ex_buf_ptr+1]=zen_kuten) ) then
@y
     e_pun_first:
        if (is_internalEUC) then
        begin
          if ((fromBUFF(ex_buf, ex_buf_length, ex_buf_ptr) = e_comma) or 
              (fromBUFF(ex_buf, ex_buf_length, ex_buf_ptr) = e_toten)) then
@z
@x
          else if(ex_buf[ex_buf_ptr+1]=zen_space) then
@y
          else if (fromBUFF(ex_buf,ex_buf_length,ex_buf_ptr) = e_space) then
@z
@x
        end;
@y
        end else begin
            ex_buf_ptr := ex_buf_ptr + multibytelen(ex_buf[ex_buf_ptr]);
            preceding_white := false;
        end;
     u_pun_first1,
     u_pun_first2:
        if (is_internalUPTEX) then
        begin
          if ((fromBUFF(ex_buf, ex_buf_length, ex_buf_ptr) = u_comma) or 
              (fromBUFF(ex_buf, ex_buf_length, ex_buf_ptr) = u_toten)) then
                begin
                  preceding_white := false;
                  and_found  := true
                end
          else if (fromBUFF(ex_buf,ex_buf_length,ex_buf_ptr) = u_space) then
               begin
                  ex_buf[ex_buf_ptr]   := space;
                  ex_buf[ex_buf_ptr+1] := space;
                  ex_buf[ex_buf_ptr+2] := space;
                  preceding_white := true;
               end;
          ex_buf_ptr := ex_buf_ptr + 3;
        end else begin
            ex_buf_ptr := ex_buf_ptr + multibytelen(ex_buf[ex_buf_ptr]);
            preceding_white := false;
        end;
@z

@x
                if( ex_buf[ex_buf_ptr] > 127 ) then
                        ex_buf_ptr := ex_buf_ptr +2
                else
                        incr(ex_buf_ptr);
@y
            ex_buf_ptr := ex_buf_ptr + multibytelen(ex_buf[ex_buf_ptr]);
@z

@x
            if name_buf[name_bf_ptr]>127 then begin
                append_ex_buf_char_and_check (name_buf[name_bf_ptr]);
                incr(name_bf_ptr);
                append_ex_buf_char_and_check (name_buf[name_bf_ptr]);
            end
            else
                append_ex_buf_char_and_check (name_buf[name_bf_ptr]);
@y
        append_ex_buf_char_and_check (name_buf[name_bf_ptr]);
        if multibytelen(name_buf[name_bf_ptr]) > 1 then
            append_ex_buf_char_and_check (name_buf[name_bf_ptr+1]);
        if multibytelen(name_buf[name_bf_ptr]) > 2 then
            append_ex_buf_char_and_check (name_buf[name_bf_ptr+2]);
        if multibytelen(name_buf[name_bf_ptr]) > 3 then
            append_ex_buf_char_and_check (name_buf[name_bf_ptr+3]);
        name_bf_ptr := name_bf_ptr + multibytelen(name_buf[name_bf_ptr])-1;
@z

@x
{ 2 bytes Kanji code break check }
@y
{ |2..4| bytes Kanji code break check }
@z
@x
    if str_pool[tps] > 127
    then tps := tps + 2
    else incr(tps);
@y
    tps := tps + multibytelen(str_pool[tps]);
@z
@x
    if str_pool[tpe] > 127
    then tpe := tpe+2
    else incr(tpe);
@y
    tpe := tpe + multibytelen(str_pool[tpe]);
@z

@x
    if str_pool[sp_ptr] >127 then begin
         append_char (str_pool[sp_ptr]); incr(sp_ptr);
         append_char (str_pool[sp_ptr]); incr(sp_ptr);
         end
    else begin
         append_char (str_pool[sp_ptr]); incr(sp_ptr);
         end;
@y
    append_char (str_pool[sp_ptr]);
    if multibytelen(str_pool[sp_ptr]) > 1 then
        append_char (str_pool[sp_ptr+1]);
    if multibytelen(str_pool[sp_ptr]) > 2 then
        append_char (str_pool[sp_ptr+2]);
    if multibytelen(str_pool[sp_ptr]) > 3 then
        append_char (str_pool[sp_ptr+3]);
    if multibytelen(str_pool[sp_ptr]) > 0 then
        sp_ptr := sp_ptr + multibytelen(str_pool[sp_ptr])
    else
        incr(sp_ptr);
@z

@x
            incr(sp_xptr1); num_text_chars:=num_text_chars+2;
@y
            num_text_chars := num_text_chars + multibytelen(str_pool[sp_xptr1-1]);
            sp_xptr1 := sp_xptr1-1 + multibytelen(str_pool[sp_xptr1-1]);
@z

@x
const n_options = 6; {Pascal won't count array lengths for us.}
@y
const n_options = 7; {Pascal won't count array lengths for us.}
@z

@x
      usage ('pbibtex');
@y
      usage ('upbibtex');
@z

@x
      usage_help (PBIBTEX_HELP, nil);
@y
      usage_help (UPBIBTEX_HELP, nil);
@z

@x
    end; {Else it was a flag; |getopt| has already done the assignment.}
@y
    end else if argument_is ('kanji-internal') then begin
      if (not (set_enc_string(nil,optarg) and
               (is_internalEUC or is_internalUPTEX))) then
        write_ln('Bad internal kanji encoding "', stringcast(optarg), '".');

    end; {Else it was a flag; |getopt| has already done the assignment.}
@z

@x
    write_ln (stderr, 'pbibtex: Need exactly one file argument.');
    usage ('pbibtex');
@y
    write_ln (stderr, 'upbibtex: Need exactly one file argument.');
    usage ('upbibtex');
@z

@x
@ An element with all zeros always ends the list.
@y
@ Kanji-internal option.
@.-kanji-internal@>

@<Define the option...@> =
long_options[current_option].name := 'kanji-internal';
long_options[current_option].has_arg := 1;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr(current_option);

@ An element with all zeros always ends the list.
@z

@x
exit:end;
@y
exit:end;

@ @<Initialize variables depending on Kanji code@>=
if (is_internalUPTEX) then
  begin
    for i:=@"80 to @"BF do lex_class[i] := alpha; { trail bytes }
    for i:=@"C0 to @"C1 do lex_class[i] := illegal;
    for i:=@"C2 to @"F4 do lex_class[i] := alpha; { lead bytes }
    for i:=@"F5 to @"FF do lex_class[i] := illegal;
    for i:=@"C0 to @"C1 do id_class[i] := illegal_id_char;
    for i:=@"F5 to @"FF do id_class[i] := illegal_id_char;
    for i:=@"80 to @"BF do char_width[i]:=257; { trail bytes }
    for i:=@"C2 to @"DF do char_width[i]:=771; { lead bytes (2bytes) }
    for i:=@"E0 to @"EF do char_width[i]:=514; { lead bytes (3bytes) }
    for i:=@"F0 to @"F4 do char_width[i]:=257; { lead bytes (4bytes) }
  end
else
  begin { is_internalEUC }
    for i:=@'200 to @'240 do lex_class[i] := illegal;
    for i:=@'241 to 254 do lex_class[i] := alpha;
    lex_class[255]:=illegal;
    for i:=@'200 to @'240 do id_class[i] := illegal_id_char;
    id_class[255]:=illegal_id_char;
    lex_class[@'33]:=alpha;
    lex_class[e_ss3]:=alpha;
    for i:=@'241 to 254 do char_width[i]:=514;
    char_width[e_ss3]:=0;
  end;
@z

