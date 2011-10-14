/* Filename: SaccFilter.c
 * Authors:	Michael Liedtke, Tim Yandl, Andrew Nuxoll
 * Created: 26 Sep 2011
 * Last Modified: 28 Sep 2011
 *
 * TODO 
 *  Implement filterCommand
 *  Make sure sensorArray and lastModified are being updated appropriately.
 *  Check for NULL objects being passed in.
 **/

public class SaccFilter
{
    //A constant to define how large the sensor array should be.
    public static final int SENSOR_LENGTH = 10;
    //A constant to define how large the sacc window should be.
    public static final int WINDOW_SIZE = 3;
    
    //sensorArray should contain the most recent UNMODIFIED sensor array.
    private char[] sensorArray;
    //lastModified should contain the most recent THINNED sensor array.
    private char[] lastModified;
    //currentWindowAdr should contain the number of the saccades frame that we should be in.
    private int currentWindowAdr;
    
    /**
     * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     * The following definitions MUST, MUST, MUST accurately reflect the definitions
     * in ../communication/communication.h PLEASE double and tripple check these
     * on a regular basis!!!!
     * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     */
    public static final int FIRST_SACC_CMD = 0x7;
    public static final int LAST_SACC_CMD = 0x7;
    public static final int CMD_SACC = 0x7;
    
    


    /**
     * The constructor will set the sensor array to all zeros.
     * it is important to initialize the sensor array in the event
     * that the calling program were to immediately ask for the current
     * sensor array.
     */
    public SaccFilter()
    {
        char[] sensorArray = new char[SENSOR_LENGTH];
        for(char c:sensorArray) {c = '0';}
        char [] lastModified = new char[SENSOR_LENGTH];
        for(char c:lastModified) {c = '0';}
        currentWindowAdr = 0;
    }

    /**
     * runFilter is used as platform to launch the filter.
     * we choose to use it as opposed to calling the filter methods directly
     * because it allows us to code multiple versions of the filter and call
     * one at a time without modifying the calling program or changing method names.
     */
    public char[] runFilter(char[] sensors)
    {
        sensorArray = sensors;
        
        if(sensorArray == null)
        {
            System.out.println("SensorData NULL");
            System.exit(1);
            return null;
        }
        
        //////////////////////////////
        //  Add filter calls bellow //
        //////////////////////////////
        
        //lastModified = reverseArray(sensors);
        //lastModified = currentWindow();
        lastModified = sFilter();
        
        //////////////////////////////
        //     END filter calls     //
        //////////////////////////////
        for(char c:sensorArray){System.out.print(c);}
        System.out.print(" => ");
        for(char c:lastModified){System.out.print(c);}
        System.out.println();
        return lastModified;
    }

    /**
     * filterCommand is intercepting commands and checking for a command to change
     * the currentWindowAdr.
     */
    public int filterCommand(int command)
    {
        if(command == CMD_SACC)
        {
            this.saccades();
        }
        return command;
    }

    /**
     * Just returns the lastModified sensor array.
     */
    public char[] getSensorArray()
    {
        if(lastModified == null)
        {
            System.out.println("SensorData NULL");
            System.exit(1);
            return null;
        }
        return lastModified;
    }
    
    
    /**
     * A helper method for filterCommand.
     * after recieving a command to change the saccades window
     * this method is called.
     * currently this method moves the windo to the right, and then wraps around.
     */
    private void saccades()
    {
        //should be 0, 1, 2, or 3. (in other words mod 4)
        currentWindowAdr = (currentWindowAdr+1) % 
            (int)(Math.ceil((SENSOR_LENGTH-1)/(double)WINDOW_SIZE));
    }
    
    /**
     * This method is for testing purposes only!!!
     * it takes a character array, reverses it,
     * and spits it back out.
     */
    private char[] reverseArray(char[] chararr)
    {
        for(int i=0; i<chararr.length/2; i++)
        {
            char temp = chararr[chararr.length-(i+1)];
            chararr[chararr.length-(i+1)] = chararr[i];
            chararr[i] = temp;
        }
        return chararr;
    }

    /**
     * currentWindow()
     * Divides the sensorArray into windows of length WINDOW_LENGTH and applies a 
     * binary label to the front of each of the windows. Each window with its binary
     * label is then pushed to the back of an array of length SENSOR_LENGTH. To preserve
     * the goal bit in the origional sensorArray, before returning the window, we resave the 
     * origional goalbit back to array position 0 in our returnArray so that Ziggarat will not
     * be confused.
     * 
     * @param input: input binary array
     * @param windowSize: size of windows to break up the input array into
     * @param currentWindow: window number to return
     * @return The divided input with a binary label appended to the front
     * returns null if the requested window is out of bounds or input is null
     */
    private char[] currentWindow(/*char[] input, int windowSize, int currentWindow*/){
     //returns null if the input is null
	    if(sensorArray == null){return null;}
	    //finding the number of windows that the input will be divided into
	    int numWindows = sensorArray.length/WINDOW_SIZE + sensorArray.length%WINDOW_SIZE; 
	    //returns null if the requested window is out of bounds or if the window size is too long/short
	    if(currentWindowAdr > numWindows-1 || currentWindowAdr < 0 || WINDOW_SIZE < 1 || 
	        WINDOW_SIZE > sensorArray.length){return null;}
	    //finds the length of; the binary label that will be appended to the front of the array
	    int binaryLength = numWindows/2 + numWindows%2;
	    //Sets up a double array to store the divided input
	    char[][] dividedInput = new char[numWindows][SENSOR_LENGTH];
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
	        
	        int storageLocation = SENSOR_LENGTH-binaryLabel.length()-WINDOW_SIZE;
	        //System.out.println("Bin Length: " + binaryLabel.length() + " stor loc: " +storageLocation );
	        for(int k=0; k<binaryLabel.length();k++){
	        	//System.out.println("k: " + k);
	            dividedInput[i][k+storageLocation] = temp[k];
	        }
	        //saves the input into the divided array in the appropriate place
	        for(int j=0; j<WINDOW_SIZE;j++){
	            if(arrayLocation<sensorArray.length){
	                dividedInput[i][j+storageLocation+binaryLabel.length()] = sensorArray[arrayLocation];
	                arrayLocation++;
	            }
	            //if at the end of the input array, insert 0's
	            else
	                dividedInput[i][j+storageLocation+binaryLabel.length()] = '0';
	        }
	    }
	    char[] returnArray = new char[sensorArray.length];
	    //sets the values in the return array
	    for(int i=0; i<SENSOR_LENGTH;i++){
	        if(i>=binaryLength+WINDOW_SIZE)
	            returnArray[i]=dividedInput[currentWindowAdr][i];
	        else
	            returnArray[i]='0';
	    }
	    //preserving goal bit
	    returnArray[0] = sensorArray[0];
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
    
    
    private char[] sFilter()
    {
        //if the currentWindowAdr is 0 and the window size is 3, then...
        //window 0 = [1,3] // binary 00
        //window 1 = [4,6] // binary 01
        //window 2 = [7,9] // binary 10
        //{G,x,x,x,x,A,A,w,w,w} where G is the Goal, x's are masked out, AA represents
        // the window address in binary, and the w's represent the bits in the window.
        
        //calculate the number of windows (round up to nearest int)
        int numWindows = (int)Math.ceil((SENSOR_LENGTH - 1.0)/WINDOW_SIZE);
        //take the log base2 of the number of windows and the round up...
        //this is the number of bits needed to represent the window address.
        int adrSize = (int)Math.ceil(Math.log(numWindows)/Math.log(2));
        char[] adr = new char[adrSize];
        // convert the address to binary
        int temp = currentWindowAdr;
        for(int i = adrSize-1; i>=0; i--)
        {
            if(temp % 2 == 0) {adr[i] = '0';}
            else {adr[i] = '1';}
            temp = temp/2; //perform an integer division to move to the next binary place.
        }
        // retrieve the current window from sensor array. if it runs of the end
        // of the sensor array, fill it with 0's
        char[] currentWindow = new char[WINDOW_SIZE];
        int windowStart = (currentWindowAdr*WINDOW_SIZE)+1;
        for(int i = 0; i<WINDOW_SIZE; i++)
        {
            currentWindow[i] = ((windowStart+i) < SENSOR_LENGTH) ? sensorArray[windowStart+i] : '0';
        }
        // put the peices together!
        //adr[0] = '0';
        //adr[1] = '0';
        char[] ret = new char[SENSOR_LENGTH];
        ret[0] = sensorArray[0]; //the goal bit is never changed and is never included in a window
        for(int i = 1; i<SENSOR_LENGTH; i++)
        {
            if(i<(SENSOR_LENGTH-(WINDOW_SIZE+adrSize))){ret[i] = '0';}
            else if(i<SENSOR_LENGTH-WINDOW_SIZE){ret[i] = adr[i-(SENSOR_LENGTH-(WINDOW_SIZE+adrSize))];}
            else {ret[i] = currentWindow[i-(SENSOR_LENGTH-WINDOW_SIZE)];}
        }
        return ret;
    }
}





