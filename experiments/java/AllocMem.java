import java.util.*;

public class AllocMem {

    public static void main(String[] args) throws InterruptedException 
    {
    	OOMExp exp;

		if( args.length == 1 )
		{
			exp = new OOMExp( Integer.parseInt( args[0] ) );
		}
		else if( args.length == 2 )
		{
			exp = new OOMExp( Integer.parseInt( args[0] ), Integer.parseInt( args[1] ) );
		}
		else
		{
			exp = new OOMExp();
		}
    	exp.Alloc();
    	return;
	}

}
