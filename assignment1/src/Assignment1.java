/* Do NOT!!! put a package statement here, that would break the build system */

public class Assignment1 {

	public static void run(long max) {
/*====================================TODO===================================*/
		for(int i = 1; i <= max; i++)
		{
			if(i % 5 == 0 && i % 3 == 0){
				
				System.out.println("FizzBuzz");
				continue;
			}	
			if(i % 5 == 0)
				System.out.println("Buzz");
			if(i % 3 == 0)
				System.out.println("Fizz");
			if(i % 5 != 0 && i % 3 != 0)
				System.out.println(i);
		}
/*===========================================================================*/
	}



	public static void main(String[] argv) {
		Arguments args = new Arguments(argv);
		try{
			run(args.max);
		}
		catch(Exception e) {
			e.printStackTrace();
			System.out.println(e.getMessage());
			System.exit(1);
		}
	}
}
