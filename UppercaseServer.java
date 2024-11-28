import java.io.*;
import java.net.*;

// javac UppercaseServer.java
// javac UppercaseClient.java
// java UppercaseServer
// java UppercaseClient

public class UppercaseServer {
    public static void main(String[] args) {
        int port = 12345;  // The port the server will listen on
        ServerSocket serverSocket = null;

        try {
            // Create a server socket
            serverSocket = new ServerSocket(port);
            System.out.println("Server is listening on port " + port);

            while (true) {
                // Accept client connection
                Socket clientSocket = serverSocket.accept();
                System.out.println("Client connected: " + clientSocket.getInetAddress().getHostAddress());

                // Create input and output streams
                BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);

                // Read the input string from the client
                String input = in.readLine();
                System.out.println("Received string from client: " + input);

                // Convert the string to uppercase
                String uppercased = input.toUpperCase();

                // Send the uppercase string back to the client
                out.println(uppercased);

                // Close the connection
                clientSocket.close();
            }
        } catch (IOException e) {
            System.err.println("Server exception: " + e.getMessage());
            e.printStackTrace();
        } finally {
            try {
                if (serverSocket != null) {
                    serverSocket.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}

