/* ------------------------------------------------------------------------- */
 /*   Copyright (C) 2017 
                Author:  Ryan Bomalaski - rbomalaski2015@my.fit.edu
                Florida Tech, Computer Science
   
       This program is free software; you can redistribute it and/or modify
       it under the terms of the GNU Affero General Public License as published by
       the Free Software Foundation; either the current version of the License, or
       (at your option) any later version.
   
      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.
  
      You should have received a copy of the GNU Affero General Public License
      along with this program; if not, write to the Free Software
      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.              */
 /* ------------------------------------------------------------------------- */
package ass5;

import gnu.getopt.Getopt;
import java.io.*;
import java.net.*;
import java.util.Base64;
import java.util.Scanner;
import javax.net.ssl.*;

/**
 *
 * @author ryanbomo
 */
public class Ass5 {

    static BufferedReader in;
    static OutputStream os;
    static SSLSocket socket;
    static int tag;
    static String tagl;
    static String tag_head;
    static String path;

    /**
     * @param argv the command line arguments
     * @throws java.io.IOException
     */
    public static void main(String[] argv)
            throws IOException {
        Ass5 imap = new Ass5();
        int c;
        String arg;

        // initialize with defaults beause it wanted me to
        String site = "default.site.mcsiteyface.com";
        String user = "admin";
        String password = "passwordistaco";
        String folder_string = "";
        boolean del_aft = false;
        boolean down_all = false;
        int port = 1234;

        Getopt g = new Getopt("testprog", argv, "S:P:l:p:daf:");
        g.setOpterr(false);

        while ((c = g.getopt()) != -1) {
            switch (c) {
                case 'S':
                    arg = g.getOptarg();
                    if (arg == null) {
                        System.out.println("Need a site. Exiting.");
                        System.exit(0);
                    }
                    site = arg;
                    break;
                //
                case 'P':
                    arg = g.getOptarg();
                    if (arg == null) {
                        System.out.println("Need a port. Exiting.");
                        System.exit(0);
                    }
                    port = Integer.parseInt(arg);
                    break;
                //
                case 'l':
                    if (g.getOptarg() != null) {
                        arg = g.getOptarg();
                        user = arg;
                    } else {
                        System.out.println("Enter your username: ");
                        Scanner scanner = new Scanner(System.in);
                        user = scanner.nextLine();
                    }
                    break;
                //
                case 'p':
                    if (g.getOptarg() != null) {
                        arg = g.getOptarg();
                        password = arg;
                    } else {
                        System.out.println("Enter your Password: ");
                        Scanner scanner = new Scanner(System.in);
                        password = scanner.nextLine();
                    }
                    break;
                //
                case 'd':
                    del_aft = true;
                    break;
                //
                case 'a':
                    down_all = true;
                    break;
                //
                case 'f':
                    arg = g.getOptarg();
                    // holding each folder location by adding to string,
                    // and delimiting with an assumed unique phrase
                    // bad practice, but such is life.
                    folder_string = folder_string + arg + "&&";
                    break;
                //
                case '?':
                    System.out.println("The option '" + (char) g.getOptopt() + "' is not valid");
                    break;
                //
                default:
                    System.out.println("getopt() returned " + c);
                    break;
            }
        }

        //file creation ping to get current folder
        File dumby = new File("");
        path = dumby.getAbsolutePath() + "/" + user + "/";
        dumby.delete();
        imap.buildFolders(user);

        //array of folder names
        String[] folders = folder_string.split("&&");

        //connect to imap server
        imap.connectIMAP(site, port, user, password);

        //download all?
        if (down_all) {
            imap.downloadFolder("", del_aft, user);
        }

        //for each folder, download that folder
        if (!down_all) {
            for (String folder : folders) {
                imap.downloadFolder(folder, del_aft, user);
            }
        }

        //logout
        imap.quickCommand(tagl + " LOGOUT\r\n");
        socket.close();
        os.close();
        in.close();
    }

    // connect to imap, initiate variables as necessary
    public void connectIMAP(String site, int port, String user, String password)
            throws IOException {
        socket = (SSLSocket) ((SSLSocketFactory) SSLSocketFactory.getDefault()).createSocket(InetAddress.getByName(site), port);
        os = socket.getOutputStream();
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        tag_head = "a0";
        tag = 0;
        updateTag();
        //send(null);
        //System.out.println(in.readLine());
        quickCommand(tagl + " LOGIN " + user + " " + password + "\r\n");

    }

    // this will build the folder, since 
    public void buildFolders(String folder) {
        // create the directory
        File theDir = new File(folder);

        // if the directory does not exist, create it
        if (!theDir.exists()) {
            try {
                theDir.mkdir();
            } catch (SecurityException se) {
                //System.out.println("oops");
            }
        }
    }

    public void downloadFolder(String folder, boolean delete, String user)
            throws IOException {
        System.out.println("Grabbing subfolders");
        String folders[] = list(folder);
        for (String subfolder : folders) {
            System.out.println("Building subfolder: " + subfolder);
            buildFolders((user + "/" + subfolder));
            //select folder
            String command = tagl + " SELECT \"" + subfolder + "\"\r\n";
            quickCommand(command);
            //get folder status
            command = tagl + " STATUS \"" + subfolder + "\" (messages)\r\n";
            int messag_num = status(command);
            //for each folder in 
            System.out.println("Populating subfolder: " + subfolder);
            if (messag_num > 0) {
                for (int i = 0; i < messag_num; i++) {
                    //write it into it's own file, filename is XXXXX_SENDER_TITLE.txt
                    String message = fetchtxt(i + 1, "text");
                    String header = fetchheader(i + 1, "header");
                    header = (i + 10000) + "_" + header;
                    buildFolders((user + "/" + subfolder + "/" + header));
                    System.out.println("Creating file: " + header + ".txt");
                    mailToTXT(message, "content", (subfolder + "/" + header));

                    //check attachment
                    if (checkAttachment(i + 1)) {
                        System.out.println("Has attachment");
                        grabAttachment((i + 1), (subfolder + "/" + header));
                        //mailToTXT("Has attachment", "attachment", (subfolder + "/" + header));
                    }

                    if (delete) {
                        quickCommand(tagl + " UID STORE " + (i + 1) + " +FLAGS (\\Deleted)\r\n");
                    }
                }
            }
            if (delete) {
                System.out.println("Deleting from: " + subfolder);
                quickCommand(tagl + " EXPUNGE\r\n");
            }
        }

    }

    //Originally had separate classes for select, logout, expunge and delete
    //realized that they were almost exaclty identical, minus the command text
    //so instead I implemented something that can quickly run commands that we
    //don't need the output for.
    public void quickCommand(String command)
            throws IOException {
        os.write(command.getBytes());
        //System.out.println(command);
        String response;
        String word = "OK"; // for response "* a3 OK..."
        String stop = "NO"; // for response "* a3 NO..."
        String bad = "BAD"; // for bad expunge
        while (true) {
            response = in.readLine();
            //System.out.println(response);
            if (response.indexOf(word) == (getTagLength() + 1)
                    || response.indexOf(stop) == (getTagLength() + 1)
                    || response.indexOf(bad) == (getTagLength() + 1)) {
                break;
            }
        }
        updateTag();
    }

    // returns a list of subfolders
    public String[] list(String folder)
            throws IOException {
        String command = tagl + " LIST \"" + folder + "\" \"*\"\r\n";
        os.write(command.getBytes());
        //System.out.println(command);
        String response;
        String word = "OK";
        String stop = "NO";
        String list = "LIST";
        String folders = "";
        String thisLine[];
        String folderList[];
        while (true) {
            response = in.readLine();
            //System.out.println(response);
            if (response.indexOf(word) == (getTagLength() + 1) || response.indexOf(stop) == (getTagLength() + 1)) {
                break;

            } else if (response.indexOf(list) == 2) {
                thisLine = response.split("\"");
                folders = folders + thisLine[3] + "&&";
            }
        }
        folderList = folders.split("&&");
        updateTag();
        return folderList;
    }

    // returns the number of messages in the folder
    public int status(String command)
            throws IOException {
        os.write(command.getBytes());
        //System.out.println(command);
        String response;
        String word = "OK";
        String stop = "NO";
        String status = "STATUS";
        int num_mes = 0;
        while (true) {
            response = in.readLine();
            //System.out.println(response);
            if (response.indexOf(word) == (getTagLength() + 1) || response.indexOf(stop) == (getTagLength() + 1)) {
                break;

            } else if (response.indexOf(status) == 2) {
                num_mes = Integer.parseInt(response.replaceAll("[\\D]", ""));

            }
        }
        updateTag();

        return num_mes;

    }

    // returns the txt to be written to the txt
    public String fetchtxt(int mes_num, String modifier)
            throws IOException {
        String command = tagl + " FETCH " + mes_num + " (BODY[" + modifier + "])\r\n";
        os.write(command.getBytes());
        //System.out.println(command);
        String response;
        String word = "OK";
        String stop = "NO";
        String status = "* " + mes_num + " FETCH";
        String message = "";
        boolean start_grokking = false;
        while (true) {
            response = in.readLine();
            //System.out.println(response);

            // check to stop
            if (response.indexOf(word) == (getTagLength() + 1) || response.indexOf(stop) == (getTagLength() + 1)) {
                break;
            }

            //check to see if we want to be grabbing text
            if (start_grokking) {
                message = message + response;
            }
            //check to see if at start
            if (response.indexOf(status) == 0) {
                start_grokking = true;
            }
        }
        updateTag();
        return message;
    }

    // checks to see if there is an attachment
    public boolean checkAttachment(int mes_num) throws IOException {
        String command = tagl + " FETCH " + mes_num + " (BODY[header])\r\n";
        os.write(command.getBytes());
        //System.out.println(command);
        String response;
        String word = "OK";
        String stop = "NO";
        String content_type = "Content-Type: multipart/mixed";
        boolean attachment = false;
        while (true) {
            response = in.readLine();
            if (response.indexOf(word) == (getTagLength() + 1) || response.indexOf(stop) == (getTagLength() + 1)) {
                break;
            } else if (response.indexOf(content_type) == 0) {
                attachment = true;
            }
        }
        updateTag();
        return attachment;

    }

    //grabs the attachment
    //Series of logic gates parsing the response
    // decodes base64 attachments
    public void grabAttachment(int mes_num, String folder) throws IOException {
        String command = tagl + " FETCH " + mes_num + " (BODY[text])\r\n";
        String attachment = "";
        os.write(command.getBytes());
        //System.out.println(command);
        String response;
        String filename = "";
        String word = "OK";
        String stop = "NO";
        boolean first_dash = true;
        String before_grab = "X-Attachment-Id:";
        boolean start_grokking = false;
        while (true) {
            response = in.readLine();
            System.out.println(response);
            if (response.indexOf(word) == (getTagLength() + 1) || response.indexOf(stop) == (getTagLength() + 1)) {
                break;
            }
            if ((response.indexOf("--") == 0) && !first_dash) {

                System.out.println("Filename is: " + filename);
                start_grokking = false;
                byte[] asBytes = Base64.getDecoder().decode(attachment);
                try (PrintWriter writer = new PrintWriter((path + folder + "/" + filename), "UTF-8")) {
                    writer.println(asBytes);
                }
                attachment = "";
            }
            if (start_grokking) {
                attachment = attachment + response;
            }
            if (response.indexOf("filename") != -1) {
                first_dash = false;
                int shift = response.indexOf("filename");
                shift = shift + ("filename=\"").length();
                System.out.println(response);
                System.out.println("Shift is: " + shift);
                for (int i = shift; i < 200; i++) {
                    char this_char = response.charAt(i);
                    if (this_char == '"') {
                        break;
                    }
                    filename = filename + this_char;
                }
            }
            if (response.indexOf(before_grab) == 0) {
                start_grokking = true;
            }

        }

        updateTag();
    }

    //grabs the header and then turns it into part of the file name
    public String fetchheader(int mes_num, String modifier)
            throws IOException {
        String command = tagl + " FETCH " + mes_num + " (BODY[" + modifier + "])\r\n";
        os.write(command.getBytes());
        //System.out.println(command);
        String response;
        String word = "OK";
        String stop = "NO";
        String subject = "Subject: ";
        String from = "From: ";
        String message_from = "";
        String message_subject = "";
        String header;
        while (true) {
            response = in.readLine();
            //System.out.println(response);
            if (response.indexOf(word) == (getTagLength() + 1) || response.indexOf(stop) == (getTagLength() + 1)) {
                break;

            } else if (response.indexOf(from) == 0) {
                response = response.replaceAll("From: ", "");
                for (int i = 0; i < response.length(); i++) {
                    char this_char = response.charAt(i);
                    if (this_char == '<') {
                        break;
                    }
                    message_from = message_from + this_char;
                }

            } else if (response.indexOf(subject) == 0) {
                response = response.replaceAll("Subject: ", "");
                for (int i = 0; i < response.length(); i++) {
                    char this_char = response.charAt(i);
                    if (this_char == '<') {
                        break;
                    }
                    message_subject = message_subject + this_char;
                }
            }
        }
        header = message_from + "_" + message_subject;
        updateTag();
        return header;
    }

    // writes txt to file
    public void mailToTXT(String mail, String filename, String folder)
            throws FileNotFoundException, UnsupportedEncodingException {
        try (PrintWriter writer = new PrintWriter((path + folder + "/" + filename + ".txt"), "UTF-8")) {
            writer.println(mail);
        }
    }

    //gets the lenght of the tag, used for parsing server responses
    public int getTagLength() {
        return tagl.length();
    }

    //updates the tag, used to make messages unique
    public void updateTag() {
        tag++;
        tagl = tag_head + tag;
    }
}
