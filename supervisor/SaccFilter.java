

public class SaccFilter
{
   //private int[] sensors; //currently unused

   public static void main(String[] args) {} // unused
   
   public SaccFilter() {} //nothing needs to be initilized yet.
   
   public int[] runFilter(int[] sensors)
   {
      return reverseArray(sensors);
   }
   
   private int[] reverseArray(int[] intarr)
   {
      for(int i=0; i<intarr.length/2; i++)
      {
         int temp = intarr[intarr.length-(i+1)];
         intarr[intarr.length-(i+1)] = intarr[i];
         intarr[i] = temp;
      }
      return intarr;
   }
   
   /**
	 * currentWindow()
	 * Takes an input array and divides it into windows with the specified size.
	 * It returns the window number corresponding to current window
	 * 
	 * @param input: input binary array
	 * @param windowSize: size of windows to break up the input array into
	 * @param currentWindow: window number to return
	 * @return The divided input with a binary label appended to the front
	 * returns null if the requested window is out of bounds or input is null
	 */
	 private int[] currentWindow(int[] input, int windowSize, int currentWindow){
		//returns null if the input is null
		if(input == null){return null;}
		//finding the number of windows that the input will be divided into
		int numWindows = input.length/windowSize + input.length%windowSize; 
		//returns null if the requested window is out of bounds or if the window size is too long/short
		if(currentWindow > numWindows-1 || currentWindow < 0 || windowSize < 1 || 
				windowSize > input.length){return null;}
		//finds the length of the binary label that will be appended to the front of the array		
		int binaryLength= numWindows/2 + numWindows%2;
		//Sets up a double array to store the divided input
		int[][] dividedInput = new int[numWindows][binaryLength+windowSize];
		//keeps track of where in the input array the loop left off at
		int arrayLocation = 0;
		//temporary string used to store a binary string
		String binaryLabel;

		//self check to make sure the arrays are being returned correctly,
		//assigns a 2 to all positions in the divided input array
		//if a 2 was returned, then the array was not assigned or returned correctly.
		/*for(int i=0; i<numWindows;i++){
			for(int j=0; j<windowSize+binaryLength;j++){
				dividedInput[i][j] = 2;
			}
		}*/

		//assigns all the positions in the divided input array
		for(int i=0; i<numWindows; i++){
			//retrieves the binary string from the helper method
			binaryLabel = getBinary(i,binaryLength);
			//sets the front of the array to the binary label
			for(int k=0; k<binaryLabel.length();k++){
				//special case for the end of the string, needs to use a different substring call.
				if(k==binaryLabel.length()-1){
					dividedInput[i][k] = Integer.parseInt(binaryLabel.substring(k));
				}
				else{
					//uses Integer static call to parse and int out of the single character substring of the 
					//temporary binary string
					dividedInput[i][k] = Integer.parseInt(binaryLabel.substring(k, k+1));
				}

			}
			//saves the input into the divided array in the appropriate place
			for(int j=0; j<+windowSize;j++){
				//check too make sure the loop doesn't go out of bounds on the input array
				if(arrayLocation<input.length){
					dividedInput[i][j+binaryLabel.length()] = input[arrayLocation];
					arrayLocation++;
				}
			}
		}
		//print check to print the contents of the double array
		/*for(int i=0; i<numWindows;i++){
			System.out.print("Line"+i+": ");
			for(int j=0; j<windowSize+binaryLength;j++){
				System.out.print(dividedInput[i][j]);
			}
			System.out.println();
		}*/

		//return conditions
		//a normal return
		if(currentWindow < numWindows-1 || input.length%windowSize==0){
			//return dividedInput[currentWindow];
		}
		//special case return where the last window was requested and a shorter array needs to be returned.
		else
		{
			//finds the size of this specific return array
			int lastArrayLength = binaryLength + input.length%windowSize;
			int[] returnArray = new int[lastArrayLength];
			//sets the values in the return array
			for(int i=0; i<lastArrayLength;i++){
				returnArray[i]=dividedInput[currentWindow][i];
			}
		//return returnArray;
		}
		int lastArrayLength = binaryLength + input.length%windowSize;
			int[] returnArray = new int[lastArrayLength];
			//sets the values in the return array
			for(int i=0; i<lastArrayLength;i++){
				returnArray[i]=dividedInput[currentWindow][i];
			}
		/* Sudo Code for alternative return (returning an array of size 10 always)
		* Comment out both returns
		* int[] actualReturnArray = new int[getSizeOfArrayFromCBridge];
		* for(int i = 0; i < actualReturnarray; i++){	
		* 	if(returnArray.length < i){
		*		actualRetrunArray[i]= returnArray[i];
		*   }
		*	else{ 
		*		actualReturnArray[i] = 0;
		*	}
		* }*/
		//returnActualReturnArray
		
		//returns the window plus appended 0's to the end of it until
		//the length is the same as the input's length. 
		int[] actualReturnArray = new int[input.length];
		 for(int i = 0; i < actualReturnArray.length; i++){	
		 	if(returnArray.length < i){
				actualReturnArray[i]= returnArray[i];
		  }
			else{ 
				actualReturnArray[i] = 0;
			}
		}
		return actualReturnArray;
		
	}
	/**
	 * getBinary()
	 * takes a number and converts it into binary with the requested amount of bits
	 * and returns the binary number in a string form to maintain leading 0's
	 *  
	 * @param num: number to turn into binary
	 * @param numBits: number of bits for the binary form
	 * @return, a string with the binary form of the input number with the 
	 * specified number of bits. 
	 */
	 private String getBinary(int num, int numBits){
		String returnString = Integer.toBinaryString(num);
		//if the binary form of the input number isn't the specified number of bits,
		//then append 0's in front until the correct length is reached. 
		for(int i=0; i<numBits; i++){
			if(returnString.length() == numBits){
				return returnString;
			}
			else{
				returnString = "0" + returnString;
			}
		}
		return null;	
	}  
}
