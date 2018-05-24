/* DO NOT!!! put a package statement here, that would break the build system */
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.HashMap;

public class Assignment2 {

/*====================================TODO===================================*/
	/* Put your required state variables here */
	static byte[] mymac;

/*===========================================================================*/

	public static void run(GRNVS_RAW sock, int frames) {
		byte[] recbuffer = new byte[1514];
		int length;
		Timeout timeout = new Timeout(10000);

/*====================================TODO===================================*/
	/* If you want to set up any data/counters before the receive loop,
	 * this is the right location
	 */
		mymac = sock.getMac();
		int end = frames;
		int multicast = 0;
		int notMulticast = 0;
		HashMap<int[],int[]> allEtherTypes = new HashMap<>();
/*===========================================================================*/

	/* keep this AS IS! the tester uses this to avoid races */
		System.out.format("I am ready!\n");

/*====================================TODO===================================*/
	/* Update loop condition */
		while (end-- > 0) {
/*===========================================================================*/
			length = sock.read(recbuffer, timeout);
			if (length == 0) {
				System.err.format("Timed out, this means there was nothing to receive. Do you have a sender set up?\n");
				break;
			}
/*====================================TODO===================================*/
	/* This is the receive loop, 'recbuffer' will contain the received
	 * frame. 'length' tells you the length of what you received.
	 * Anything that should be done with every frame that's received
	 * should be done here.
	 */
			int[] addTo = allEtherTypes.get(new int[]{recbuffer[12],recbuffer[13]});
			if(addTo != null)
			{
				allEtherTypes.put(new int[]{recbuffer[12],recbuffer[13]},new int[]{addTo[0] + 1,addTo[1] + 1});
			}
			else
			{
				allEtherTypes.put(new int[]{recbuffer[12],recbuffer[13]},new int[]{1, 1});
			}
			if(recbuffer[0] > 127){

				notMulticast++;
			}
			else{

				multicast++;
			}
/*===========================================================================*/
		}
/*====================================TODO===================================*/
	/* Print your summary here */
		if (allEtherTypes.get(new int[]{8,0}) == null) {
			allEtherTypes.put(new int[]{8,0},new int[]{0,0});
		}
		if (allEtherTypes.get(new int[]{8,6}) == null) {
			allEtherTypes.put(new int[]{8,6},new int[]{0,0});
		}
		if (allEtherTypes.get(new int[]{134,221}) == null) {
			allEtherTypes.put(new int[]{134,221},new int[]{0,0});
		}
		if(allEtherTypes.get(new int[]{8,0})[0] > 0){

			System.out.printf("0x0800: %d frames, %d bytes\n", allEtherTypes.get(new int[]{8,0})[0], allEtherTypes.get(new int[]{8,0})[1]);
		}
		if(allEtherTypes.get(new int[]{8,6})[0] > 0){

			System.out.printf("0x0806: %d frames, %d bytes\n", allEtherTypes.get(new int[]{8,6})[0], allEtherTypes.get(new int[]{8,6})[1]);
		}
		if(allEtherTypes.get(new int[]{134,221})[0] > 0){

			System.out.printf("0x86dd: %d frames, %d bytes\n", allEtherTypes.get(new int[]{134,221})[0], allEtherTypes.get(new int[]{134,221})[1]);
		}
		System.out.printf("%d of them were for me\n", notMulticast);
		System.out.printf("%d of them were multicast\n", multicast);
		int allBytes = allEtherTypes.get(new int[]{8,0})[1] + allEtherTypes.get(new int[]{8,6})[1] + allEtherTypes.get(new int[]{134,221})[1];
		if(allBytes > 0){
			System.out.printf("IPv4 accounted for %f%% and IPv6 for %f%% of traffic\n", 100.0 * allEtherTypes.get(new int[]{8,0})[1]/allBytes, 100.0 * allEtherTypes.get(new int[]{134,221})[1]/allBytes);
		}
/*===========================================================================*/
	}


	public static void main(String[] argv) {
		Arguments args = new Arguments(argv);
		GRNVS_RAW sock = null;
		try{
			sock = new GRNVS_RAW(args.iface, 3);
			run(sock, args.frames);
			sock.close();
		}
		catch(Exception e) {
			e.printStackTrace();
			System.out.println(e.getMessage());
			System.exit(1);
		}
	}
}
