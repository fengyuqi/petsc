#! /usr/local/tcl/bin/tclsh

##################################################
####              scanfile()                 #####    
##################################################

proc scanfile { filename } {
    global Keywords

    # open the file
    set fileid [ open $filename r ]
    set filebuff [ read $fileid ]
    close $fileid
    
    set start_comment [ string first "<H2>Keywords</H2>"  $filebuff ]
    if { $start_comment == -1 } { 
        set mesg "No Keywords found in $filename"
        #puts stderr $mesg
        return 0
    }
    # Elimitate the stuff up to "Keywords"
    set filebuff [ string range $filebuff $start_comment end ]
    set start_comment [ string first "\n"  $filebuff ]
    # Eliminate the line containing "Keywords"
    set filebuff [ string range $filebuff [expr $start_comment +1 ] end ]
    # Grab all the keywords. The delimiter is either "<P>"
    set start_comment [ string first "<P>"  $filebuff ]
    set filebuff [ string range $filebuff 0  [expr $start_comment -1] ]
    # In case there is a HTML LINE BREAK - "<BR>" take it away
    regsub -all  "<BR>" $filebuff "," filebuff
    # take away any newline chars present and replace it by ","
    set filebuff [join [ split $filebuff "\n" ] "," ]

    set filebuff "$filebuff,"
    regsub -all {,+ *} $filebuff  "," filebuff
    regsub -all {,+} $filebuff  "," filebuff
    set start_comment [ string last ","  $filebuff ]
    set filebuff [ string range $filebuff 0  [expr $start_comment  -1] ]

#    puts $filename
#    puts $filebuff
    set Keywords($filename)  [ split $filebuff "," ] 

    return 0
}

##################################################
####           deletespace()                 #####    
##################################################

proc deletespace { name } { 
    upvar $name buf 
    #Delete spaces from the list buf
    
    set n [ llength $buf ]
    set i 0
    set new {}
    while { $i < $n } {
        set temp [ join [ lindex $buf $i ] " " ]
        set temp  [string trim $temp ]
        lappend new $temp
        set i [ expr $i + 1 ]
        
    }
    set buf $new
    return 0
}
##################################################
####       write_keywords_file()             #####    
##################################################


proc write_keywords_file { } {
    global  keywords Keywords Keywordsfunction 
    global  PETSC_DIR html
    
    set PETSC_DIR ../..
#    set PETSC_DIR ftp://info.mcs.anl.gov/pub/petsc/petsc
    exec /bin/rm -f docs/www/keywords.html
    set keywords_file [ open docs/www/keywords.html w ]

    # Put some  HTML Header 
    puts $keywords_file {<HTML>}
    puts $keywords_file {<TITLE>Keywords_File</TITLE>}
    puts $keywords_file {<BODY>} 
    
    # Put the Table Header
    puts $keywords_file {<H1> Keywords Index </H1>}
    
    # Puts Tabular Header
    puts $keywords_file {<TABLE>}
    puts $keywords_file {<TR HEIGHT=10>}
    puts $keywords_file {<TH WIDTH=4 ><BR></TH>}
    puts $keywords_file {<TH WIDTH=192 ><B><I><FONT SIZE=5>Keywords</FONT></B></I></TH>}
    puts $keywords_file {<TH WIDTH=132 ><B><I><FONT SIZE=5>Functions</FONT></B></I></TH>}
    puts $keywords_file {</TR>}
    puts $keywords_file {</TABLE>}


    foreach keyword  $keywords {
        set n [ llength $Keywordsfunction($keyword)  ]
        puts $keywords_file {<TABLE>}
        puts $keywords_file {<TD WIDTH=4 ><BR></TD>}
        puts $keywords_file {<TD WIDTH=1000 ><I><FONT SIZE=5>}
        puts $keywords_file $keyword
        puts $keywords_file {</FONT></I></TD>}
        puts $keywords_file {</TR>}
        puts $keywords_file {</TABLE>}
        
        set i 0
        while { $i < $n } {
            set functionname [ join [ lindex $Keywordsfunction($keyword) $i ] " " ]
            #set temp [ format "<A HREF=\"%s/%s\">%s</A>" $PETSC_DIR $filename $filename ]
            puts $keywords_file {<TABLE>}
            puts $keywords_file {<TD WIDTH=192 ><BR></TD>}
            puts $keywords_file {<TD WIDTH=300 >}
            puts $keywords_file $html($functionname)
            puts $keywords_file {</TD>}
            puts $keywords_file {</TR>}
            puts $keywords_file {</TABLE>}
            
            set i [ expr $i + 1 ]
        }
    }

    # HTML Tail
    puts $keywords_file {</BODY>} 
    puts $keywords_file {</HTML>}
    
    close $keywords_file
    exec /bin/chmod ug+w docs/www/keywords.html
    return 0
}

##################################################
####               main()                    #####    
##################################################
# Initialise some global datastructures
# change dir to PETSC_DIR
proc main { }  {
    global Keywords Keywordsfunction keywords keyword html

    set PETSC_DIR /home/bsmith/petsc
    cd $PETSC_DIR

    set files [ glob docs/www/man*/*.html]
    set keywords {}
    set functionnames {}

    foreach filename $files { 
        set Keywords($filename) {}
        set html($filename) {}
        scanfile $filename
        deletespace Keywords($filename) 
        #puts $Keywords($filename)
    }
    foreach filename $files {
        # extract the function name frim the file name
        set functionname [ file tail $filename ]
        regsub -all ".html" $functionname "()" functionname
        #set Filename($functionname) $filename
        set Functionname($filename) $functionname
        lappend functionnames $functionname
    }
 
    # Do the grouping by Keywords
    foreach filename $files {
        set functionname $Functionname($filename)
        foreach keyword $Keywords($filename) {
            lappend Keywordsfunction($keyword) $functionname
            # add to the keywords list
            if { [lsearch -exact $keywords $keyword ] == -1 } {
                #puts stderr $keywords
                lappend keywords $keyword
            }
        }
    }

    # sort the keys and also the contents
    set keywords [ lsort $keywords ]
    foreach keyword $keywords {
        set Keywordsfunction($keyword) [ lsort $Keywordsfunction($keyword) ]
    }

    # Modify the filename and make it hypertext 
    set PETSC_DIR  ../..
    foreach filename $files {
        set functionname $Functionname($filename)
        set tmp [ format "<A HREF=\"%s/%s\">%s</A>" $PETSC_DIR $filename $functionname ]
        #puts $tmp
        set html($functionname) $tmp
    }
    #puts $keywords

    # write the output file
    write_keywords_file

    return 0
}
 
#################################################
# the stupid main function is called here    
main
