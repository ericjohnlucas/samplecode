
public class palindrome{

    //Given a string which contains one or more palindrome substrings, this method will return the length of the longest substring
    public static int returnLongestPalindromeLength(String s){
        int longest_length=-1;
        //we iterate through the string
        for (int i=0; i<s.length(); i++){
            //if we find two consecutive characters that are equal, we have found a palindrome of even length
            if (i>0 && s.charAt(i)==s.charAt(i-1)){
                int length=2;
                int j=i-2;
                int k=i+1;
                //we iterate in either direction to see how long the palindrome is
                while(j>=0 && k<s.length()){
                    if (s.charAt(j)!=s.charAt(k)){
                        break;
                    } else{
                        length+=2;
                        j--; k++;
                    }
                }
                if (length>longest_length) longest_length=length;
            }
            //if we find a character that is equal to the character two indexes backwards, we have found a palindrome of odd length
            if (i>1 && s.charAt(i)==s.charAt(i-2)){
                int length=3;
                int j=i-3;
                int k=i+1;
                //we iterate in either direction to see how long the palindrome is
                while(j>=0 && k<s.length()){
                    if (s.charAt(j)!=s.charAt(k)){
                        break;
                    } else{
                        length+=2;
                        j--; k++;
                    }
                }
                if (length>longest_length) longest_length=length;
            }
        }
        return longest_length;
    }
    
    public static void main(String args[]) throws Exception{
        System.out.println(returnLongestPalindromeLength("xxyyabxbaaaa"));
    }
    
}
  
