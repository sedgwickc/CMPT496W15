import java.util.*;

public class AllocMem {

    public static void main(String[] args) throws InterruptedException 
    {
    	List<byte[]> mem_chunks = new ArrayList<byte[]>();

    	System.out.println("Allocating Memory...");
    	while(true)
    	{
    		mem_chunks.add( new byte[1048576 * 50] );
    		Thread.sleep(500);
    	}
	}

}
