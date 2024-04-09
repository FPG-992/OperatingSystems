After forking: In the child process 
Close the unused ends of pipes 
Loop for reading tasks from parents

Parent process: Close unused ends of pipes 
Loop for distributing tasks to children  
                    and reading results

Wait should be in the end of code before 
return 0 in a for loop (<N) 

at the end free the allocated memory
for the pipes using free()
