public class Writer extends Thread {
    @Override
    public void run() {
        while (Buffer.var < Buffer.MAX){
            try {
                Thread.sleep(700);
                Buffer.semaphore.acquire(2);

                System.out.println("The writer acquires the lock.");

                Buffer.var++;

                System.out.println("The writer writes the value " + Buffer.var + ".");

            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("The writer releases the lock.");

            Buffer.semaphore.release(2);
        }
    }
}