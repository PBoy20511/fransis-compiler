# fransis-compiler
fransis compiler
此程式為Project 3，能將source code轉為中間碼的形式。我主要的資料結構為token的struct(裡面有字元的內容name，它的subroutine，它的type，它的pointer，基本上就是identifier table理的內容)和之後要輸出用的struct(裡面有第幾line，operators，operand1，operand2，result，code，errorMsg)。Table 1 Delimiters和Table 2 (Reserved Word Table)所有的內容我直接寫死在程式裡面。
