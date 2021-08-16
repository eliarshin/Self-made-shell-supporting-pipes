# Self-made-shell-supporting-pipes
Self made shell supporting pipes

***********IMPORTANT************
Hello, my program had a problem to read >> and 2> , but i still wanted to do the functions so i replaced >> and 2> with other symbols.
I thought it is more important to do their functions instead dont do them at all.
REPLACEMNT SYMBOLS ARE:

1) Instead 2> use @   use @ to make redirection for std error.
Example : abcd@errors      /     ls -l | ls@error

2)insted >> use #    use # to make redirection or create new text or add information for text that already have been created
Example: ls -l#newOne      /   echo hello world#existedOne

Again, im sorry that not following the rules (i really did not have time to change all my structure to this) but the functions are working hope you will consider it, thanks!.
***********IMPORTANT************

About the code:
The code creates selfmade shell that support shell functions and include into it also Pipe functions and ridirections.
There are several types of options(>,<,#,@,|) to write our functions and get the result.
We can get information and in the same time assume it into a text created/existed already.
Also there is a register log that register all the funcitons we used.
The shell do not support funciton "cd".
To finish the program we should write done.



How to compile:
1)Open terminal in the current folder that include the code.
2)write ./ex2 log.txt
The funciton ./ex2 will open the code and our code will be directed to log.txt and saved there.

Otherwise you can run through Visual studio but log.txt needed to run the program, we get it as argument on the main.

How to run and what is the input:

It is simple program code that makes a new shell with max input by size 510, we got several funcitons that we can run.
1) '|' Pipe function that can run two functions in same time.
for example : echo hello world | wc -w 
this code will return us how many words in the echo.

2) '>' Create new document and if he already exsisted so to delete all there and make a new one with our information.
 for example : ls -l>Files this code will return us the inforamtion to new created text called Files

3) '<' Get information from exsisted file already, it will be printed on the terminal , we can sort it or what ever we want. 
example : sort < Files
 this code will sort the information on Files.

***********IMPORTANT************
4)'@' Create new document that will send all errors to them instead them to be printed on the screen.
Example: aaaa@errors   dkwodko@errors 
This code will redirect the errors to the errors instead them being printed.

5)'#'Create new document or add to existed one information.
Example:Echo hello world#Files
whis code will add hello world to files if existed or will create new one called files with hello world inside
***********IMPORTANT************

6)'ls -l | dada@errors' this will make both things, also print ls -l for the current thing and will create error path to errors
Example:ls -l | aaaa@errors



Result output:
The output will give us :
1)How many commands written
2)The total length of the commands
3)The average length of every command.
4)How many pipes created
5)how many ridirections used
6)log file with all our commands
7)Depend on the function it will create or ridirect things to another
