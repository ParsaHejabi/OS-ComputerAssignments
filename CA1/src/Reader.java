public class Reader extends Thread {

    private String readerName;

    public Reader(String name) {
        super(name);
        readerName = name;
    }

    @Override
    public void run() {
        while (Buffer.var < Buffer.MAX){
            try {
                Thread.sleep(500);
                Buffer.readerSemaphore.acquire();
                Buffer.semaphore.acquire();

                if (Buffer.semaphore.availablePermits() == 1)
                    System.out.println("The first reader acquires the lock.");
                Buffer.readerSemaphore.release();

                if (readerName.equals("Reader1"))
                    System.out.println("Reader1 reads the value " + Buffer.var + ".");
                else
                    System.out.println("Reader2 reads the value " + Buffer.var + ".");

            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            Buffer.semaphore.release();

            if (Buffer.semaphore.availablePermits() == 2)
                System.out.println("The last reader releases the lock.");
        }
    }
}