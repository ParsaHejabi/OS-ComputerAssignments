public class CA1 {
    public static void main(String[] args) throws InterruptedException {

        Writer writer = new Writer();
        Reader reader1 = new Reader("Reader1");
        Reader reader2 = new Reader("Reader2");

        writer.start();
        reader1.start();
        reader2.start();

        writer.join();
        reader1.join();
        Thread.sleep(1000);
        reader2.join();

        System.out.println("Program finished.");
        System.out.println("Buffer var: " + Buffer.var);
    }
}