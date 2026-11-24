public class Main {
    public static void main(String[] args) {
        int x;
        int y;
        int result;

        x = 10;
        y = 20;
        result = x * y + 5;

        if (result >= 100) {
            System.out.println("Result exceeds threshold");
        } else {
            System.out.println("Result within range");
        }
    }
}
