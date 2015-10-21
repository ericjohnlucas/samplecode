Public class Sample {

    ////prints all permutations of the input string. iterative approach using queue
    public static void printpermsqueue(String s){
        Queue<String> q = new LinkedList<String>();
        q.add(s.substring(0,1));
        while(q.size()>0){
            String dequeue =q.remove();
            if (dequeue.length()==s.length()){
                System.out.println(dequeue);
            } else {
                String nxt = s.substring(dequeue.length(),dequeue.length()+1);
                for (int i=0; i<=dequeue.length(); i++){
                    q.add(dequeue.substring(0,i)+nxt+dequeue.substring(i,dequeue.length()));
                }
            }
        }
    }
    
    //prints all permutations of the input string. iterative approach using stack
    public static void printpermsstack(String s){
        Stack<String> q = new Stack<String>();
        q.push(s.substring(0,1));
        while(!q.empty()){
            String pop = q.pop();
            if (pop.length()==s.length()){
                System.out.println(pop);
            } else {
                String nxt = s.substring(pop.length(),pop.length()+1);
                for (int i=0; i<=pop.length(); i++){
                    q.add(pop.substring(0,i)+nxt+pop.substring(i,pop.length()));
                }
            }
        }
    }
    
    //Concider a square grid of size n*n where a robot starts at the top left corner
    //This methos will return the total number of paths the robot can take to arrive at the bottom left
    public static int numberofpaths(int n,int x,int y){
        if ((x == (n-1)) && (y== (n-1))){
            return 1;
        } else if (x== (n-1)){
            return 1;
        } else if (y== (n-1)){
            return 1;
        } else {
            return (numberofpaths(n,x+1,y)+numberofpaths(n,x,y+1));
        }
    }
    

    //prints the digit of an integer number from the greatest place value to the least place value with a digit on each line.
    //No string or character methods are used. Iterative approach starting with the largest 32-bit place value
    public static void printvertically(int i){
        int mod =1000000000;
        boolean start_printing=false;
        while(true){
            int nxt= i/mod;
            if ((nxt > 0) || (start_printing==true)){
                System.out.println(nxt);
                i = i - ((nxt)*mod);
                start_printing=true;
            }
            if (mod!=1){
                mod =mod/10;
            } else {
                break;
            }
        }
    }
    
    //prints the digit of an integer number from the greatest place value to the least place value with a digit on each line.
    //No string or character methods are used. Recursive approach
    public static void printvertically2(int i){
        int rem=i%10;
        i-=rem;
        if (i>0) printvertically(i/10);
        else return;
        System.out.println(rem);
    }
    
    //Given two strings which denote version numbers, this method will return
    // a)a negative number if the first number is less then the second,
    // b)a positive number if the first number is greater then the second,
    // c)or a zero if the two version numbers are equal
    public static int compareVersionNumbers(String v1, String v2) throws Exception{
        int startindex=0;
        int indexofperiod1=v1.indexOf('.');
        int indexofperiod2=v2.indexOf('.');
        if ((indexofperiod1==-1)||(indexofperiod2==-1)){
            if ((indexofperiod1!=-1)||(indexofperiod2!=-1)){
                throw new Exception("The two version numbers given have a different number of sections which are seperated by a '.' ");
            }
            if (Integer.parseInt(v1)>Integer.parseInt(v2)){
                return 1;
            } else if (Integer.parseInt(v1)<Integer.parseInt(v2)){
                return -1;
            } else {
                return 0;
            }
        }
        String p1=v1.substring(0,indexofperiod1);
        String p2=v2.substring(0,indexofperiod2);
        if (Integer.parseInt(p1)>Integer.parseInt(p2)){
            return 1;
        } else if (Integer.parseInt(p1)<Integer.parseInt(p2)){
            return -1;
        }
        return compareVersionNumbers(v1.substring(indexofperiod1+1,v1.length()),v2.substring(indexofperiod2+1,v2.length()));
    }
    
}
