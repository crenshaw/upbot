import java.util.Arrays;

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
***************/ public static final boolean DEBUG = true ; /*******************
*******************************************************************************/
    
    private final boolean useWindowAdr = false;
    
    /**
     * Note about variables and constants with regard to style:
     *
     *  because this program is operating as a part of of program written in C
     *  We have chosen to have constants and variables follow the java convention
     *  iff it is used entirely within the java code, however if the constant or
     *  variable is reflecting a corresponding constant or variable in the main
     *  program, it will follow the C convention. This mixed convention
     *  should assist in readablity and debugging.
     */
    
    public static final int FIRST_SACC_CMD = 0x7;   //found in ../communication.h
    public static final int LAST_SACC_CMD = 0xA;    //found in ../communication.h
    public static final int CMD_SACC_1 = 0x7;       //found in ../communication.h
    public static final int CMD_SACC_2 = 0x8;       //found in ../communication.h
    public static final int CMD_SACC_3 = 0x9;       //found in ../communication.h
    public static final int CMD_SACC_4 = 0xA;       //found in ../communication.h
    public static final int SENSOR_LENGTH = 10;     //
    
    // Window description is an array which indicates where and how large windows should be.
    private final int[][] windowDescriptions = {{1,2,3},{4,5},{6,7},{8,9}};
    /*
     * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> WARNING <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     * the program makes no check for the validity of the windowDescriptions.
     * 
     * Remember: the number of windows effects the Address Size.
     * Remember: the goal bit may not be overwritten, effectivly shrinking SENSOR_LENGTH by 1
     * Remember: Address Size = lg(numWindows) rounded up
     * General rules:
     *   1. The description must have atleast 1 window.
     *   2. No window may not be larger than SENSOR_LENGTH-(1+AddressLength)
     * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> WARNING <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     */
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
        char [] lastModified = new char[SENSOR_LENGTH];
        Arrays.fill(sensorArray, '0');
        Arrays.fill(lastModified, '0');
        currentWindowAdr = 0;
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
        if(sensors == null)
        {
            System.err.println("SensorData NULL");
            return null;
        }
        
        sensorArray = sensors;
        
        //////////////////////////////
        //  Add filter calls bellow //
        //////////////////////////////
        
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
        int lastWindow = currentWindowAdr;
        if(command == CMD_SACC_1)
            currentWindowAdr = 0;
        else if(command == CMD_SACC_2)
            currentWindowAdr = 1;
        else if(command == CMD_SACC_3)
            currentWindowAdr = 2;
        else if(command == CMD_SACC_4)
            currentWindowAdr = 3;
        //currentWindowAdr = (currentWindowAdr+1)%windowDescriptions.length;
        if(DEBUG)
        {
            if(FIRST_SACC_CMD <= command && command <= LAST_SACC_CMD)
                System.out.println("Window Adr changed: " + lastWindow + " => " + currentWindowAdr);
            else
                System.out.println("Window Adr unchange: " + currentWindowAdr);
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
            System.err.println("SensorData NULL");
            return null;
        }
        return lastModified;
    }
    
    /**
     * A filter method variation that does not ever include the goalbit.
     *
     *
     * @return the thinned version of the most recent sensorArray.
     */
    private char[] sFilter()
    {
        //{G,x,x,x,x,A,A,w,w,w} where G is the Goal, x's are masked out, AA represents
        // the window address in binary, and the w's represent the bits in the window.
        //take the log base2 of the number of windows and the round up...
        //this is the number of bits needed to represent the window address.
        int adrSize = (useWindowAdr)?(int)Math.ceil(Math.log(windowDescriptions.length)/Math.log(2)):0;
        char[] adr = new char[adrSize];
        // convert the address to binary
        int temp = currentWindowAdr;
        for(int i = adrSize-1; i>=0; i--)
        {
            if(temp % 2 == 0) {adr[i] = '0';}
            else {adr[i] = '1';}
            temp = temp/2; //perform an integer division to move to the next binary place.
        }
        // retrieve the current window from sensor array.
        char[] currentWindow = new char[windowDescriptions[currentWindowAdr].length];
        for(int i = 0; i<currentWindow.length; i++)
            currentWindow[i] = sensorArray[windowDescriptions[currentWindowAdr][i]];
        // put the peices together!
        char[] ret = new char[SENSOR_LENGTH];
        char[] tempArray = Arrays.copyOf(adr, adr.length + currentWindow.length);
        System.arraycopy(currentWindow, 0, tempArray, adr.length, currentWindow.length);
        if(tempArray.length+1>(SENSOR_LENGTH))
        {
            System.err.println("Too much info for output Array.\nNOT APPLYING FILTER!");
            return sensorArray;
        }
        Arrays.fill(ret, '0');
        ret[0] = sensorArray[0];
        System.arraycopy(tempArray, 0, ret, ret.length - tempArray.length, tempArray.length);
        return ret;
    }
}






