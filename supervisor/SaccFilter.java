

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
	private char[] currentWindow(char[] input, int windowSize, int currentWindow){
	//returns null if the input is null
	if(input == null){return null;}
	//finding the number of windows that the input will be divided into
	int numWindows = input.length/windowSize + input.length%windowSize; 
	//returns null if the requested window is out of bounds or if the window size is too long/short
	if(currentWindow > numWindows-1 || currentWindow < 0 || windowSize < 1 || 
			windowSize > input.length){return null;}
	//finds the length of; the binary label that will be appended to the front of the array		
	int binaryLength = numWindows/2 + numWindows%2;
	//Sets up a double array to store the divided input
	char[][] dividedInput = new char[numWindows][binaryLength+windowSize];
	//keeps track of where in the input array the loop left off at
	int arrayLocation = 0;
	//temporary string used to store a binary string
	String binaryLabel;
	char[] temp;
	//assigns all the positions in the divided input array
	for(int i=0; i<numWindows; i++){
		//retrieves the binary string from the helper method
		binaryLabel = getBinary(i,binaryLength);
		temp = binaryLabel.toCharArray();
		//sets the front of the array to the binary label
		for(int k=0; k<binaryLabel.length();k++){
			dividedInput[i][k] = temp[k];
		}
		//saves the input into the divided array in the appropriate place
		for(int j=0; j<+windowSize;j++){
			if(arrayLocation<input.length){
				dividedInput[i][j+binaryLabel.length()] = input[arrayLocation];
				arrayLocation++;
			}
			//if at the end of the input array, insert 0's
			else
				dividedInput[i][j+binaryLabel.length()] = '0';
		}
	}
	char[] returnArray = new char[input.length];
	//sets the values in the return array
	for(int i=0; i<input.length;i++){
		if(i<dividedInput[currentWindow].length)
			returnArray[i]=dividedInput[currentWindow][i];
		else
			returnArray[i]='0';
	}
	return returnArray;
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
				returnString = '0' + returnString;
			}
		}
		return null;	
	}  
}
