import java.util.*;

public class OOMExp 
{
	private List<byte[]> mem_blocks;
	private int mem_allocd;
	private static final int ONE_MB = 1048576;
	private int block_size;
	private int period; // in ms

	public OOMExp()
	{
		mem_blocks = new ArrayList<byte[]>();
		block_size = 50;
		mem_allocd = 0;
		period = 500;
	}
	
	public OOMExp( int bsize )
	{
		mem_blocks = new ArrayList<byte[]>();
		block_size = bsize;
		mem_allocd = 0;
		period = 500;
	}

	public OOMExp( int bsize, int p )
	{
		mem_blocks = new ArrayList<byte[]>();
		block_size = bsize;
		mem_allocd = 0;
		period = p;
	}
	public void Alloc() throws InterruptedException
	{
		int size = ONE_MB * block_size;
    	while(true)
    	{
    		try
    		{
    			mem_blocks.add( new byte[ size ] );
				mem_allocd += block_size;
    			Thread.sleep(period);
    		} 
    		catch (OutOfMemoryError E) 
    		{
    			System.out.println( "Out of Memory! Memory allocated: " + mem_allocd + "MB" );
    			return;
    		}
    	}
	}
}
