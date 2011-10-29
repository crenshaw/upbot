/* Filename: SaccFilter.c
 * Authors:	Michael Liedtke, Tim Yandl, Andrew Nuxoll
 * Created: 26 Sep 2011
 * Last Modified: 28 Sep 2011
 *
 * TODO 
 *  
 **/

public class SaccFilter
{
/***************************** DEBUG MODE **************************************
***************/ public static final boolean DEBUG = true; /*******************
*******************************************************************************/

    private final boolean useWindowAdr = true;

    /**
     * Note about variables and constants with regard to style:
     *
     *  because this program is operating as a part of of program written in C
     *  We have chosen to have constants and variables follow the java convension
     *  iff it is used entirely within the java code, however if the constant or
     *  variable is reflecting a corresponding constant or variable in the main
     *  program, it will follow the convension (all caps). This mixed convension
     *  should assist in readablity and debugging.
     */
    
    public static final int FIRST_SACC_CMD = 0x7;   //found in ../communication.h
    public static final int LAST_SACC_CMD = 0x7;    //found in ../communication.h
    public static final int CMD_SACC = 0x7;         //found in ../communication.h
    
    public static final int SENSOR_LENGTH = 10;     //
    
    public static final int windowSize = 3; //the number of sensor bits per window
    
    private int currentWindowAdr; //indicates which window currently "has focus"
    
    private char[] sensorArray; //contains a recent unmodified sensor array.
    private char[] lastModified; //contains a recent thinned sensor array.
    
    /**
     * The constructor will set the sensor array to all zeros.
     * it is important to initialize the sensor array in the event
     * that the calling program were to immediately ask for the current
     * sensor array.
     */
    public SaccFilter()
    {
        //initilize the sensorArray, and then make last modified the same.
        char[] sensorArray = new char[SENSOR_LENGTH];
        for(char c:sensorArray) {c = '0';}
        char [] lastModified = sensorArray;
        
        currentWindowAdr = 0; //the first window always starts with focus
    }

    /**
     * runFilter is used as platform to launch the filter.
     * we choose to use it as opposed to calling the filter methods directly
     * because it allows us to code multiple versions of the filter and call
     * one at a time without modifying the calling program or changing method names.
     * 
     * @param the newest sensor array, which will be stored into sensorArray
     * @return the thinned version of the most recent sensorArray.
     */
    public char[] runFilter(char[] sensors)
    {
        sensorArray = sensors;
        
        if(sensorArray == null)
        {
            System.out.println("SensorData NULL");
            return null;
        }
        
        //////////////////////////////
        //  Add filter calls bellow //
        //////////////////////////////
        
        //lastModified = currentWindow();
        lastModified = sFilter();
        
        //////////////////////////////
        //     END filter calls     //
        //////////////////////////////
        
        if(DEBUG)
        {
            for(char c:sensorArray){System.out.print(c);}
            System.out.print(" => ");
            for(char c:lastModified){System.out.print(c);}
            System.out.println();
        }
        
        return lastModified;
    }

    /**
     * filterCommand is intercepting commands and checking for a command to change
     * the currentWindowAdr.
     * 
     * @param the command we are intercepting
     * @return the unchanged command that was passed in.
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
     * 
     * @return the most recent sensor array.
     */
    public char[] getSensorArray()
    {
        if(lastModified == null)
        {
            System.out.println("SensorData NULL");
            return null;
        }
        return lastModified;
    }
    
    
    /**
     * A helper method for filterCommand.
     * after recieving a command to change the saccades window
     * this method is called.
     * currently this method moves the windo to the right, and then wraps around.
     * 
     * @return void
     */
    private void saccades()
    {
        //should be 0, 1, 2, or 3. (in other words mod 4)
        currentWindowAdr = (currentWindowAdr+1) % 
            (int)(Math.ceil((SENSOR_LENGTH-1)/(double)windowSize));
    }
    
    /**
     * A filter method variation that does not ever include the goalbit.
     *
     *
     * @return the thinned version of the most recent sensorArray.
     */
    private char[] sFilter()
    {
        //if the currentWindowAdr is 0 and the window size is 3, then...
        //window 0 = [1,3] // binary 00
        //window 1 = [4,6] // binary 01
        //window 2 = [7,9] // binary 10
        //{G,x,x,x,x,A,A,w,w,w} where G is the Goal, x's are masked out, AA represents
        // the window address in binary, and the w's represent the bits in the window.
        
        //calculate the number of windows (round up to nearest int)
        int numWindows = (int)Math.ceil((SENSOR_LENGTH - 1.0)/windowSize);
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
        char[] currentWindow = new char[windowSize];
        int windowStart = (currentWindowAdr*windowSize)+1;
        for(int i = 0; i<windowSize; i++)
        {
            currentWindow[i] = ((windowStart+i) < SENSOR_LENGTH) ? sensorArray[windowStart+i] : '0';
        }
        // put the peices together!
        if(!useWindowAdr)
        {
            for(int i=0;i<adrSize;i++)
            {
                adr[i] = '0';
            }
        }
        char[] ret = new char[SENSOR_LENGTH];
        ret[0] = sensorArray[0]; //the goal bit is never changed and is never included in a window
        for(int i = 1; i<SENSOR_LENGTH; i++)
        {
            if(i<(SENSOR_LENGTH-(windowSize+adrSize))){ret[i] = '0';}
            else if(i<SENSOR_LENGTH-windowSize){ret[i] = adr[i-(SENSOR_LENGTH-(windowSize+adrSize))];}
            else {ret[i] = currentWindow[i-(SENSOR_LENGTH-windowSize)];}
        }
        return ret;
    }
}





