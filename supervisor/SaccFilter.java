

public class Filter
{
   //private int[] sensors; //currently unused

   public static void main(String[] args) {} // unused
   
   public Filter() {} //nothing needs to be initilized yet.
   
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
}
