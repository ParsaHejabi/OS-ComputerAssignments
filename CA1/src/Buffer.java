import java.util.concurrent.Semaphore;

public class Buffer {

    public static Semaphore readerSemaphore = new Semaphore(1);
    public static Semaphore semaphore = new Semaphore(2);
    public static int var = 0;
    public static final int MAX = 5;

}