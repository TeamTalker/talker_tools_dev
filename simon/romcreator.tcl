set width 293 ;# in pixels
set height 178 ;# in pixels
wm geometry . ${width}x$height
wm resizable . 0 0
wm title . "ROMCreator"
. configure
frame .c
frame .c.f -borderwidth 0 -relief sunken -width 200 -height 20
 
label .c.l1 -text "Enter ROM Name, Author and Description:"
entry .c.e -width 30 -textvariable romname -validate key -validatecommand {expr [string length %P]<33}  -bg gray97
.c.e insert 0 "ROM Name (max 32 chars)"
entry .c.d -width 30 -textvariable romauthor -validate key -validatecommand {expr [string length %P]<33}  -bg gray97
.c.d insert 0 "Author (max 32 chars)"
entry .c.text -width 30 -textvariable romdescription -validate key -validatecommand {expr [string length %P]<129}  -bg gray97
.c.text insert 0 "Description (max 128 chars)"
button .c.b -text "Choose LPC Bin Directory" -command { getDirectory } -relief flat -overrelief raised
button .c.close -text "Close" -command exit -relief flat -overrelief raised

grid .c -row 0 -column 0
grid .c.l1 -row 0 -column 0 -columnspan 2
#grid .c.f -column 0 -row 0 -columnspan 2 -pady 5 -padx 5
grid .c.e -row 1 -column 0  -columnspan 2 -pady 5 -padx 5
grid .c.d -row 2 -column 0  -columnspan 2 -pady 5 -padx 5

grid .c.text -column 0 -row 3 -columnspan 2  -pady 5 -padx 5
grid .c.b  -column 0 -row 4 -pady 3 -padx 2
grid .c.close -column 1 -row 4 -pady 3 -padx 2
#get directory with worddata, extract all necessary data, fill  arrays and variables ask for filename and save...
proc getDirectory {} {
    set dir [tk_chooseDirectory -initialdir ~ -title "Choose LPC Wordfiles Directory"]
    
# 4byte size of rom 4byte major 4byte point 32 byte romname 32byte romauthor 128byte romdescription 4byte constant size 4byte table_id 4byte wordcount 4byte lpc_entrypoint 128byte unused space
set rommetasize 356
#filetype
set filetype vsm2    
# romversion
set major  1 
set point  0 
# 
set constframe 0
set lpcid 7
# not yet used, maybe in future revision, 4byte for increment, 128byte for unknown things
set increment 0
set hastones 0  
set unusedblock 0

# 4 byte length 4byte increment 16byte wordspelling 4byte entrypoint
set wordmetasize 28

set files [glob -nocomplain -types f $dir/*.bin]
set i 0
set fullsize 0 
set onsets 0
foreach f $files {
set size [file size $f]
set sizearray($i) $size
 
set name [file rootname [file tail $f]]
set wordfull [lindex [split "$name" "_"] 1]
if {$wordfull eq ""} {
    set word [string range $name 0 15]
} else {
     set word [string range $wordfull 0 15]
}
set namearray($i) $word
set fp [open  $f r]
fconfigure $fp -translation binary
set binarray($i) [read  $fp]
close $fp
if {$i} {
set idec [expr {$i -1}]
set onsets [expr {$onsets + $sizearray($idec)}]
}
set relativeentryarray($i) $onsets
set fullsize [expr {$fullsize + $size}]
incr i
}
set entrypoint [expr {$i*$wordmetasize + $rommetasize}]
set lpcsize $fullsize
set fullsize [expr {$fullsize + $entrypoint}]
for {set j 0} {$j < $i} {incr j} {
set absoluteentryarray($j) [expr {$relativeentryarray($j) + $entrypoint} ]
}
if {$i} {
set savefile [tk_getSaveFile -initialdir ~ -title "Save ROM As"]
set rommetadata [binary format a4i2i1a32a32a128i2i1i1i1a128 [list $filetype] [list $major $point ] [list $fullsize ] [split $::romname] [split $::romauthor] [split $::romdescription] [list $constframe $lpcid] [list $lpcsize] [list $hastones] [list $i] [list $unusedblock] ]

set fbin [open $savefile.vsm2 w]
fconfigure $fbin -translation binary
puts -nonewline $fbin $rommetadata
for {set k 0} {$k < $i} {incr k} {
set wordmetadata [binary format i1i1a16i1 [list $sizearray($k)] [list $increment] [list $namearray($k)] [list $absoluteentryarray($k)]  ]
puts -nonewline $fbin $wordmetadata
}
for {set l 0} {$l < $i} {incr l} {
    puts -nonewline $fbin $binarray($l)
    }
close $fbin
  }  
#only for debug
#foreach {key value} [array get sizearray] {
 #   puts "$key => $value"
#}
#foreach {key value} [array get namearray] {
 #   puts "$key => $value"
#}
#foreach {key value} [array get absoluteentryarray] {
#	puts "$key => $value"
#}
#puts "$i"
#puts "$entrypoint"
#puts  "$fullsize"
}
 
