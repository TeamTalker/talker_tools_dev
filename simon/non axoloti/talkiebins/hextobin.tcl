#convert hex values (with 0x removed in texteditor) to bin files, extracting name of word from textfile as well
set fp [open lpctclhex.txt r]
set lines [split [read $fp] "\n"]
close $fp
foreach line $lines {
    set filename [lindex [split "$line" "="] 0]
    set filename [regsub -all { } $filename ""]
    set filename [string tolower $filename]
    set data [lindex [split "$line" "="] 1]
    set fsave [open $filename.bin w]
    fconfigure $fsave -translation binary
    set byte [split $data " "]
    foreach line $byte {
	puts -nonewline $fsave  [binary format H* $line]
    }
    close $fsave
}
