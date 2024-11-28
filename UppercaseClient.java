import java.io.*;
import java.net.*;

public class UppercaseClient {
    public static void main(String[] args) {
        String serverAddress = "localhost"; // Server address (localhost for local testing)
        int serverPort = 12345; // Server port (must match the server's port)

        try {
            // Create a socket and connect to the server
            Socket socket = new Socket(serverAddress, serverPort);
            System.out.println("Connected to server at " + serverAddress + ":" + serverPort);

            // Create input and output streams for the socket
            BufferedReader userInput = new BufferedReader(new InputStreamReader(System.in));
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            // Get a string from the user to send to the server
            System.out.print("Enter a string to convert to uppercase: ");
            String inputString = userInput.readLine();

            // Send the string to the server
            out.println(inputString);

            // Receive the uppercase string from the server
            String uppercasedString = in.readLine();
            System.out.println("Uppercase string from server: " + uppercasedString);

            // Close the socket
            socket.close();
        } catch (IOException e) {
            System.err.println("Client exception: " + e.getMessage());
            e.printStackTrace();
        }
    }
}

