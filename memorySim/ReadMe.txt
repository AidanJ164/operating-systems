Aidan Justice
Memory Simulator
4/12/24
CSC 458 - Operating Systems and Virtual Machines
Running the Program:
This program is entirely created with Python. To run the program, simply use:

        python main.py

Python 3.10 or higher is recommended as this was what it was developed on, however, it has been shown to work with Python 3.7.


Program Design:
The memory managers created for this program act as individual classes all derived from a parent MemoryManager class. 
This MemoryManager class contains all the necessary information about the memory and processes that each memory management 
method needs, such as, size of the memory, a list of all the processes, which processes are currently running, and 
processes in queue waiting to be put in memory.


The memory map is represented by a simple 2d array where the inner array describes the details of a space in memory. Space[0] 
represents the position in memory it starts at, space[1] represents the position of memory it ends at, space[2] represents the 
name of the space, and space[3] is the size of the space for VSP for easy lookup or the page or segment number for PAG and SEG 
respectively. I chose this method to represent the memory map mainly because I wanted the format to stay relatively consistent 
between the management methods because I wanted a common parent class. For paging, I could’ve represented it as a fixed array 
with the appropriate amount of pages and a data structure/class that represented each page. This method would make lookup and 
allocation relatively easy, however, I wanted the code to be similar to the other methods.


The process for allocating memory for each process is the same for each method. First, the program asks for memory size, management 
policy, either fit algorithm or page size, and then a filename that contains information about all processes that will be run. Next, 
the program will create the appropriate memory manager and start it. 


When started, the memory manager will first find the time of when the first process starts. It will look for any additional processes 
that wish to start at that time and add them to the queue. After the memory manager is invoked and finishes either adding the processes 
to memory or tries to, it will look for the time the next event happens. This event can either be a process getting freed from memory 
or another process being added to the input queue. It finds any additional processes that want to join the queue and any processes 
that will finish at that time and then invokes the memory manager to do work. This cycle continues until all processes have run and finished.


When the memory manager is invoked, it loops through the input queue and tries to add each process to memory. It will first do a check to 
see if the process will fit into memory using the appropriate policy and algorithm and if it passes this check, it will move on to being 
added to memory. This check and add to memory is different for each policy, hence the reason for a different class for each policy. After 
it attempts to add each process to memory, it will loop through the list of processes that have finished running and will free the memory 
that the process took up. Since there will be more space after the process is freed, it will then attempt to allocate memory for any 
processes in the input queue.
        
Finally, after all the processes have run, the program will output the average turnaround time of all the processes. After a process finishes 
running, it will add its turnaround time to a variable that holds the total turnaround time for all processes. When a function call for 
getting the average turnaround time is called, it returns that total turnaround time divided by the total number of processes that were run.