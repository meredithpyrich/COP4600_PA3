// Mostly borrowed Molloy's test code....
// Added a way to set the amoount of characters read back out!
// Can output null characters now!
// Changed BUFFER_LENGTH

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 1024      
static char receive[BUFFER_LENGTH];

int main(){
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   printf("Starting device test code example...\n");
   fd = open("/dev/character", O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
   printf("How many characters would you like to write?\n");
    int num;
   scanf("%d", &num);

   printf("Type in a short string to send to the kernel module:\n");
   scanf(" %[^\n]%*c", stringToSend);                // Read in a string (with spaces)
   printf("Writing message to the device [%s].\n", stringToSend);
   ret = write(fd, stringToSend, num); // Send the string to the LKM
   if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
   }
   printf("How many characters would you like?\n"); 
   scanf(" %d", &num);

   printf("Press ENTER to read back from the device...\n");
   getchar();

   printf("Reading from the device...\n");
   ret = read(fd, receive, num);        // Read the response from the LKM
   if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   int counter;
   for(counter = 0; counter < num; counter++)
   {
      putchar(receive[counter]);
   }

   printf("\n");
   printf("The received message is: [%s]\n", receive);
   printf("End of the program\n");
   return 0;
}
