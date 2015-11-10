/*******************************************************************************
  MPFS2 Utility for Microchip TCP/IP Stack
    mpfs_img2.bin, mpfs_img2.c, mpfs_img2.s, FileRcrd.bin, DynRcrd.bin
    generation.

  Company:
    Microchip Technology Inc.

  File Name:
    MPFS2Lib.java

  Summary:
    

  Description:
    Developed in NetBeans IDE 8.0, Java version 1.8.0
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

package MicrochipMPFS;
import java.util.*;
import java.io.*;
import java.util.zip.*;
import java.text.*;

public class MPFS2Lib //: MPFS2Writer
{
    MainMPFS  mainMpfs;
    public String LocalPath;
    public String LocalFile;
    public String SourcePath;
    private Collection<String> dynamicTypes;
    private Collection<String> nonGZipTypes;
    private DynVar dynVarParser;
    public List<String> log;
    public List<MPFSFileRecord> files;
    protected boolean indexUpdated;
    
    public FileOutputStream file_output;
    public DataOutputStream data_out;
    public int MPFS2_FLAG_ISZIPPED = 0x0001;
    public int MPFS2_FLAG_HASINDEX = 0x0002;
    //public static long ImageLength=0;
    public static String ASCIILine;
    public static String emptyStr;
    public String generatedImageName;
    public List<File> deeperList = new ArrayList<File>();
    public String DirPathString;
    //public Collection dynTypes;
    public MPFS2Lib()
    {
      // do nothing
    }
    public enum MPFSOutputFormat
    {
        BIN,
        C18,
        ASM30,
        C32,
        MDD,
    }

/**
  * Recursively walk a directory tree and return a List of all
  * Files found; the List is sorted using File.compareTo().
  *
  * @param aStartingDir is a valid directory, which can be read.
  */
  public List<File> getFileListing( File aStartingDir
                                          ) throws FileNotFoundException {
    DirPathString = aStartingDir.getPath();
    List<File> result = getFileListingNoSort(aStartingDir);

    //Collections.sort(result);
    //Collections.reverse(deeperList);
    
    return result;
  }

  // PRIVATE //
    public List<File> getFileListingNoSort(File aStartingDir
    ) throws FileNotFoundException {
    List<File> result = new ArrayList<File>();
    File[] filesAndDirs = aStartingDir.listFiles();
    List<File> filesDirs = Arrays.asList(filesAndDirs);
    //List<File> deeperList = new ArrayList<File>();
    //result.add(aStartingDir);
    for(File file : filesDirs) {
        if(file.isHidden())
            continue;
        if ( ! file.isFile() ) {
        //must be a directory
        //recursive call!       
        deeperList = getFileListingNoSort(file);        
        //result.addAll(deeperList);
      }
      else
        result.add(file); //always add, even if directory
    }
//    if(DirPathString.contains(aStartingDir.getPath())==true)
//        Collections.sort(deeperList);
    result.addAll(deeperList);
    return result;
  }

/*
 * File matching
 */
    private boolean FileMatches(String fileName, Collection<String> endings)
    {
        for(String end : endings)
            if(fileName.endsWith(end))
                return true;
        return false;
    }

    public void MPFS2Builder(String localPath, String localFile)//,String sourcePath)
    {
        if(!localPath.endsWith("\\"))
            localPath += File.separator;

        this.LocalPath = localPath;
        this.LocalFile = localFile;
        //this.SourcePath = sourcePath;
        this.dynamicTypes = new ArrayList<String>();
        this.nonGZipTypes = new ArrayList<String>();
        this.log = new ArrayList<String>();
        this.files = new LinkedList<MPFSFileRecord>();
        this.dynVarParser = new MicrochipMPFS.DynVar(localPath);
        this.indexUpdated = false;
    }

    public String GeneratedImageFileName()
    {
        return this.generatedImageName;
    }

    /// <summary>
    /// Sets a comma-separated list of types to be considered dynamic
    /// </summary>
    public void DynamicTypes(String dynFiles)
    {
        String[] str = dynFiles.split(",");
        this.dynamicTypes.clear();
        for(String s:str)
        {
            String s_trimmed = s.replace('*',' ').trim();
            if(s_trimmed.length() > 0)
                this.dynamicTypes.add(s_trimmed);
        }
    }

    /// <summary>
    /// Sets a comma-separated list of types not to be compressed
    /// </summary>
    public void NonGZipTypes(String nonGZipFiles)
    {
        String[] str = nonGZipFiles.split(",");
        this.nonGZipTypes.clear();
        for(String s:str)
        {
            String s_trimmed = s.replace('*',' ').trim();
            if(s_trimmed.length() > 0)
                this.nonGZipTypes.add(s_trimmed);
        }
    }

    /// <summary>
    /// Adds a file to the MPFS image
    /// </summary>
    /// <param name="localName">Local file name to read with absolute pathpath</param>
    /// <param name="imageName">Name to use in image file</param>
    public boolean AddFile(String localName, String imageName,File file)
    {
        //String strLine;
        int   byteRead;
        int count=0;
        StringBuilder strLine=new StringBuilder();
        char[] buf ;
        byte[] fileData;
        //File file = new File(localName);
        // Skip if can't be opened
        if (!file.exists())
        {
            log.add("\r\nERROR: Could not read " + localName);
            return false;
        }
        int fileSizeLength = (int)file.length();
        if(fileSizeLength < 10000000)
        {
            fileData = new byte[fileSizeLength];
            buf = new char[fileSizeLength];
        }
        else
        {
            fileData = new byte[100000];
            buf = new char[100000];
        }

        // Set up the new file record
        MPFSFileRecord newFile = new MPFSFileRecord();
        newFile.SetFileName(imageName);//FileName = imageName;
        newFile.SetFiledate(file.lastModified());

        // Read the data in
        try{
        FileInputStream inputFile = new FileInputStream(localName);

        DataInputStream in = new DataInputStream(inputFile);
        BufferedReader br = new BufferedReader(new InputStreamReader(in));

        int byteCnt=0;

        while((byteRead = br.read(buf))!= -1)
        {
            if(byteRead == 0) break;
            byteCnt++;
            strLine.append(buf);
        }
        br.close();
        inputFile = new FileInputStream(localName);
        byteRead = 0;
        ByteArrayOutputStream tempout = new ByteArrayOutputStream();
        while((byteRead = inputFile.read(fileData))!= -1)
        {
           if(byteRead == 0) break;
           tempout.write(fileData);
        }
        newFile.data = tempout.toByteArray();
        newFile.fileSizeLen = tempout.size();

        inputFile.close();
        // Parse the file if necessary
        MPFSFileRecord idxFile = null;
        if (this.FileMatches(localName, this.dynamicTypes))
        {
            idxFile = dynVarParser.Parse(newFile,strLine);
        }

        // GZip the file if possible
        int gzipRatio = 0;
        if (idxFile == null && !this.FileMatches(localName, this.nonGZipTypes))
        {
            inputFile = new FileInputStream(localName);
           /*ZipOutputStream zipout = new ZipOutputStream(new FileOutputStream("output.zip"));
            zipout.putNextEntry(new ZipEntry("output.zip"));*/
            GZIPOutputStream zipout  =  new GZIPOutputStream(new
                                        FileOutputStream("output.gzip"));
            if(fileSizeLength < 10000000)
            {
                fileData = new byte[fileSizeLength];               
            }
            else
            {
                fileData = new byte[100000];             
            }
            while((byteRead = inputFile.read(fileData)) != -1)
            {
                if(byteRead == 0) break;
                zipout.write(fileData,0,byteRead);
            }
            zipout.finish();
//            byteRead = inputFile.read();
//            while (byteRead != -1){
//            zipout.write(byteRead);
//            byteRead = inputFile.read();
//            }
            //zipout.closeEntry();
            zipout.close();
            File file_zip = new File("output.gzip");
//            if(file_zip.isFile())
//                System.out.println("original:"+file.length()+
//                        "   zip file length" +file_zip.getName()+file_zip.length()) ;
            FileInputStream inputZipFile = new FileInputStream("output.gzip");

            // Only use zipped copy if it's smaller
            if ((file_zip.length() < newFile.fileSizeLen) && (file_zip.length() > 0))
            {
                ByteArrayInputStream tempNewFileDataArray =
                        new ByteArrayInputStream(newFile.data);
                gzipRatio = (int)(100 - (100 * file_zip.length() / newFile.fileSizeLen));
                if(file_zip.length() < 100000)
                {
                    fileData = new byte[(int)file_zip.length()];
                }
                else
                {
                    fileData = new byte[100000];
                }
                //newFile.data.clear();
                tempNewFileDataArray.reset();
                tempout = new ByteArrayOutputStream();
                while((byteRead=inputZipFile.read(fileData))!= -1)
                {
                    if(byteRead == 0) break;
                    tempout.write(fileData);
                }
                newFile.data = tempout.toByteArray();
                newFile.fileSizeLen = tempout.size();
                newFile.isZipped = true;
            }
            inputZipFile.close();
            file_zip.delete();
            inputFile.close();
        }

        // Add the file and return
        if (idxFile == null)
        {
            log.add("    " + imageName + ": " + newFile.fileSizeLen + " bytes" +
                ((gzipRatio > 0) ? " (gzipped by " + gzipRatio + "%)" : ""));
            files.add(newFile);
        }
        else
        {
            log.add("    " + imageName + ": " + newFile.fileSizeLen + " bytes, " + (idxFile.fileSizeLen / 8) + " vars");
            newFile.hasIndex = true;
            files.add(newFile);
            files.add(idxFile);
        }
        }catch(IOException e){
         System.out.println ("IO exception = " + e );
         //log.add("ERROR: " + e.getMessage());
        }
        return true;
    }

    public boolean AddDirectory(String dataPath)
    {
       String imagePath="",tempImagePath="";
       String parentPath = "";
      // String rootFilePath;
        List<File> ListFiles;
        File dir = new File(dataPath);
        if(dir.exists() == false)
        {
           log.add("\r\nERROR: Directory " + dataPath + " does not exist.");
           return false;
        }
        try{
            ListFiles = getFileListing(dir);
        }catch(IOException e)
        {
            log.add("\r\nERROR: Directory " + dataPath + " does not exist.");
            return false;
        }

        // Make sure directory is not the project directory
        if (this.LocalPath.contains(dataPath))
        {
            log.add("<html>\r\nERROR: The project directory is located <br>in the source " +
                    "directory.  The generator cannot run if <br>the image is to be placed " +
                    "in the source directory. <br> Please select the base MPLAB project " +
                    "<br>directory before continuing.</html>");
            return false;
        }
        //rootFilePath = ListFiles.get(0).getPath();

        for (File f : ListFiles)
        {
           if(f.isHidden())
           {
              continue;
           }
           if (f.isDirectory())
           {
              //AddDirectory(f.getPath());
//              log.add(f.getPath() + " :");
//               if(dataPath.contains(f.getPath()) == false)
//               {
//                  if((tempImagePath.length() > 0) &&(f.getPath().contains(tempImagePath.split("/")[0])))
//                    tempImagePath = tempImagePath+"/" + f.getName();
//                  else
//                    tempImagePath = f.getName();
//
//                 imagePath = tempImagePath;
//                 imagePath = imagePath+"/";
//               }
           }
           else
           {
               if(f.getParent().compareTo(parentPath) != 0)
                log.add(f.getParent() + " :");
               tempImagePath = f.getPath();
               imagePath = tempImagePath.replace(dataPath,"").replace("\\","/");
               imagePath = imagePath.substring(1);
               //imagePath = imagePath+"/";
               //AddFile(f.getPath(),imagePath+f.getName(),f);
               AddFile(f.getPath(),imagePath,f);
               parentPath = f.getParent();
           }
        }
    
        return true;
    }

    /// <summary>
    /// Generates an image in the specified format
    /// </summary>
    /// <param name="format">One of the MPFSOutputFormat constants indicating the format</param>
    /// <returns>true if generation was successful, false otherwise</returns>
    public boolean Generate(MPFSOutputFormat format)
    {
        File temp_file;
        // Start with nothing as the output name
        generatedImageName = null;

        // Make sure all paths exist
        if (!new File(LocalPath).exists())
        {
            log.add("ERROR: Output directory \"" + LocalPath + "\" does not exist!");
            return false;
        }

        // Make sure we have some files
        if (files.size() == 0)
            return false;

        // Write any index files that have changed
        indexUpdated = dynVarParser.WriteIndices();
      
        // Determine address of each file and string
        int numFiles = (int)files.size();
        int lenHeader = 8;
        int lenHashes = 2 * numFiles;
        int lenFAT = 22 * numFiles;
        int baseAddr = lenHeader + lenHashes + lenFAT;
        int counter=0;
        int loopCntr=0;
        int numFileRecrds = 0;

        for(MPFSFileRecord file : files)
        {
            file.locStr = baseAddr;
            baseAddr += (int)file.GetFileName().length()+1;//FileName.Length + 1;
        }
        for (MPFSFileRecord file : files)
        {
            file.locData = baseAddr;
            baseAddr += (int)file.fileSizeLen;
        }

        // Set up the writer
        try
        {
            MPFS2Writer w;
            switch (format)
            {
                case C18:
                case C32:
                    //MPFS2C18Writer mpfs2C18Wr = new MPFS2C18Writer(this.LocalPath + this.LocalFile);
                    //WriteImage(mpfs2C18Wr);
                    w = new MPFS2C18Writer(LocalPath + LocalFile);
                    WriteImage(w);
                    break;
                case ASM30:
                    w = new MPFS2ASM30Writer(LocalPath + LocalFile);
                    WriteImage(w);
                    break;
                case MDD:
                    MDDWriter(LocalPath);
                    break;
                default:
                    w = new MPFS2BINWriter(LocalPath + LocalFile);
                    WriteImage(w);
                    break;
            }
       }catch (Exception e){
       log.add("ERROR: " + e.getMessage());
       return false;
       }
       return true;
    }

    private void WriteImage(MPFS2Writer x)
    {
        //byte[] hashByte= new byte[2];
        long timeVal=(long)621355968000000000L;
        long epoch = 0L;
        MPFS2Writer w;
        w = x;
        w.Write("MPFS");
        w.Write((byte)0x02);
        w.Write((byte)0x01);
        w.Write((short)(files.size()));

        for(MPFSFileRecord file : files)
        {
           //byte[0]
           w.Write((byte)(file.nameHash));
           w.Write((byte)(file.nameHash>>8));
        }

        int flags;
        for(MPFSFileRecord file : files)
        {
            w.Write(file.locStr);
            w.Write(file.locData);
            w.Write(file.fileSizeLen);
            epoch = file.fileDate/1000;
            w.Write((int)(epoch));
            w.Write((int)0);
            flags = 0;
            if (file.hasIndex)
                flags |= MPFS2_FLAG_HASINDEX;
            if (file.isZipped)
                flags |= MPFS2_FLAG_ISZIPPED;
            w.Write((short)(flags));
            timeVal=(long)621355968000000000L;
        }
        for(MPFSFileRecord file : files)
        {
            w.Write(file.GetFileName());
            w.Write((byte)(0x00));
        }

        for(MPFSFileRecord file : files)
        {
            for(int i=0;i<file.fileSizeLen;i++)
            w.Write(file.data[i]);
        }
        w.Close();
        generatedImageName = w.imageName;

        log.add("GENERATED MPFS2 IMAGE: " + w.ImageLength + " bytes");
    }

  abstract class MPFS2Writer
  {
    public int ImageLength = 0;
    public String imageName;

    public abstract void Write(byte data);
    public abstract void Close();

    public void Write(byte[] data)
    {
        for(byte b : data)
            Write(b);
    }

    public void Write(String data)
    {
        for(byte b : data.getBytes())
        Write(b);
    }

    public void Write(short data)
    {
        Write((byte)(data));
        Write((byte)(data >> 8));
    }

    public void Write(int data)
    {
        Write((byte)(data));
        Write((byte)(data >> 8));
        Write((byte)(data >> 16));
        Write((byte)(data >> 24));
    }
    public void Write(long data)
    {
        Write((byte)(data));
        Write((byte)(data >> 8));
        Write((byte)(data >> 16));
        Write((byte)(data >> 24));
        Write((byte)(data >> 32));
        Write((byte)(data >> 40));
        Write((byte)(data >> 48));
        Write((byte)(data >> 56));
       
    }
  }

  public class MPFS2C18Writer extends MPFS2Writer {
   
    public FileOutputStream file_output;
    public DataOutputStream data_out;
    public BufferedWriter fout;
    public MPFS2C18Writer(String filename)
    {
        Date date = new Date();
        SimpleDateFormat sdf = new SimpleDateFormat("E MMM dd yyyy hh:mm:ss");
        if (!filename.endsWith(".c"))//.equalsIgnoreCase(".C")))
            filename += ".c";
        try{
        // Create an output stream to the file.
        file_output = new FileOutputStream (filename);
        // Wrap the FileOutputStream with a DataOutputStream
        data_out = new DataOutputStream (file_output);
        fout = new BufferedWriter(new OutputStreamWriter(data_out));
        
        imageName = filename;
        fout.write(
            "/*****************************************************************************\r\n" +
            "  AUTO-GENERATED CODE:  Microchip MPFS Generator version: 2.2.2\r\n" +
            "\r\n" +
            "  Microchip TCP/IP Stack Application Demo\r\n" +
            "\r\n" +
            "  Company:\r\n" +
            "    Microchip Technology Inc.\r\n" +
            "\r\n" +
            "  File Name:\r\n" +
            "    mpfs_img2.c\r\n" +
            "\r\n" +
            "  Summary:\r\n" +
            "    NOT FOR HAND MODIFICATION.\r\n" +
            "    This file is automatically generated by the MPFS Generator Utility.\r\n" +
            "    ALL MODIFICATIONS WILL BE OVERWRITTEN BY THE MPFS GENERATOR.\r\n" +
            "    Generated " + sdf.format(date) + " " + "\r\n" +
            "\r\n" +
            "  Description:\r\n" +
            "    Defines an MPFS2 image to be stored in program memory.\r\n" +
            " *****************************************************************************/\r\n\r\n" +

            "// DOM-IGNORE-BEGIN\r\n" +
            "/*****************************************************************************\r\n" +
            "Software License Agreement\r\n" +
            "Copyright(c) 2014 Microchip Technology Inc. All rights reserved.\r\n" +
            "Microchip licenses to you the right to use, modify, copy and distribute\r\n" +
            "Software only when embedded on a Microchip microcontroller or digital\r\n" +
            "signal controller that is integrated into your product or third party\r\n" +
            "product (pursuant to the sublicense terms in the accompanying license\r\n" +
            "agreement).\r\n" +
            "\r\n" +
            "You should refer to the license agreement accompanying this Software\r\n" +
            "for additional information regarding your rights and obligations.\r\n" +
            "\r\n" +
            "SOFTWARE AND DOCUMENTATION ARE PROVIDED \"AS IS\" WITHOUT WARRANTY OF ANY\r\n" +
            "KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY\r\n" +
            "OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR\r\n" +
            "PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR\r\n" +
            "OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,\r\n" +
            "BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT\r\n" +
            "DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,\r\n" +
            "INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,\r\n" +
            "COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY\r\n" +
            "CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),\r\n" +
            "OR OTHER SIMILAR COSTS.\r\n" +
            " *****************************************************************************/\r\n" +
            "// DOM-IGNORE-END\r\n\r\n" +

            "#define __MPFS_IMG2_C_\r\n\r\n" +

            "#include \"system_config.h\"\r\n\r\n" +

            "#if !defined(MPFS_USE_EEPROM) && !defined(MPFS_USE_SPI_FLASH)\r\n" +
            "#include \"tcpip/tcpip.h\"\r\n\r\n" +

            "#if defined(STACK_USE_MPFS2)\r\n\r\n" +

            "/**************************************\r\n" +
            " * MPFS2 Image Data\r\n" +
            " **************************************/"
        );
        }catch (IOException e){}
    }

    public void Write(byte data)
    {
        char ASCIIdata;

        String tempStr;
        int temp=0;
        try{
        ASCIIdata = '.';
        if (data >= 32 && data <= 126 && data != '*')   // * cannot be displayed because it would open the possibility of */ occuring in the sequence, which would mess up our block comment
            ASCIIdata = (char)data;

        if (ImageLength % 1024 == 0)
            ASCIILine = " " + ASCIILine;
        else
            fout.write(",");
        if (ImageLength % 16 == 0)
        {
            if (ImageLength != 0)
                fout.write(ASCIILine + " */");
            ASCIILine = " /* ";
        }
        temp = (int)(ImageLength / 1024);
       
        tempStr = String.format("%06x", temp);
        if (ImageLength % 1024 == 0)
            fout.write("\r\n#define DATACHUNK"+tempStr);
        if (ImageLength % 16 == 0)
            fout.write(" \\\r\n\t");
        ASCIILine += Character.toString(ASCIIdata);
        tempStr = String.format("%02x", data);

        fout.write("0x" + tempStr);
        ImageLength++;
        }catch (IOException e) {
            System.out.println ("IO exception = " + e );
            log.add("ERROR: " + e.getMessage());
        }
    }
    public void Close()
    {
       String tempStr = " ";
       int lineLen=0;

       try{
       if (ImageLength % 16 != 0)
       {
           lineLen = (16-(ImageLength % 16))*5+1;
           String  formatString = "%"+lineLen+"c";
           emptyStr = " ";
           //tempStr = String.format(formatString,' ');
           //tempStr = tempStr+String.format("%20s",ASCIILine);
           tempStr = padLeft(emptyStr,lineLen);
           tempStr = tempStr+padRight(ASCIILine,20);
           tempStr = tempStr+" */";
           //tempStr = String.format("%20s",ASCIILine+" */");
           fout.write(tempStr);
       }

       fout.write("\r\n\r\n");
       if (ImageLength != 0)
       {
            fout.write(
            "/**************************************\r\n" +
            " * MPFS2 C linkable symbols\r\n" +
            " **************************************/\r\n" +
            "// For old C18 compiler, these are split into seperate arrays because it speeds up compilation a lot.  \r\n" +
            "// For other compilers, the entire data array must be defined as a single variable to \r\n" +
            "// ensure that the linker does not reorder the data chunks in Flash when compiler \r\n" +
            "// optimizations are turned on.\r\n" +
            "// Now we are using the new XC8 compiler, and we simply keep this tradition here.\r\n" +
            "#if defined(__XC8)\r\n" +
            "\tROM uint8_t MPFS_Start[] = {DATACHUNK000000};\r\n");

            for (int i = 1024; i < ImageLength; i += 1024)
            {
                tempStr = String.format("%06x",(i/1024));
                tempStr = tempStr+"[] = {DATACHUNK";
                tempStr = tempStr+String.format("%06x",(i/1024));
                tempStr = "\tROM uint8_t MPFS_"+ tempStr+"};\r\n" ;
                fout.write(tempStr);
            }

            fout.write("#else\r\n" +
                       "\tROM uint8_t MPFS_Start[] = {");
            for (int i = 0; i < ImageLength; i += 1024)
            {
                tempStr = "DATACHUNK" + String.format( "%06x",(i/1024));
                fout.write(tempStr);
                if ((i + 1024) < ImageLength)
                   fout.write(", ");
            }
            fout.write("};\r\n" +
                       "#endif\r\n\r\n");
       }

       fout.write(
            "/**************************************************************\r\n" +
            " * End of MPFS\r\n" +
            " **************************************************************/\r\n\r\n" +

            "#endif // #if defined(STACK_USE_MPFS2)\r\n\r\n" +

            "#endif // #if !defined(MPFS_USE_EEPROM) && !defined(MPFS_USE_SPI_FLASH)\r\n"
       );
       fout.flush();
       fout.close();
       } catch (IOException e) {
            System.out.println ("IO exception = " + e );
            log.add("ERROR: " + e.getMessage());
       }
    }
  }

  public class MPFS2BINWriter extends MPFS2Writer
  {
    public FileOutputStream file_output;
    public DataOutputStream data_out;
    public BufferedWriter fout;
    public ByteArrayOutputStream binOut = new ByteArrayOutputStream();
    public MPFS2BINWriter(String filename)
    {
        //Date date = new Date();
        //SimpleDateFormat sdf = new SimpleDateFormat("E MMM dd yyyy hh:mm:ss");
        if (!filename.endsWith(".bin"))//.equalsIgnoreCase(".C")))
            filename += ".bin";
        try{
        // Create an output stream to the file.
        file_output = new FileOutputStream (filename);
       
        }catch (IOException e) {
            System.out.println ("IO exception = " + e );
            log.add("ERROR: " + e.getMessage());
        }
        imageName = filename;
    }

    public void Write(byte data)
    {
        //try{
        ImageLength++;
        //file_output.write(data);
        binOut.write(data);
//        }catch (IOException e) {
//            System.out.println ("IO exception = " + e );
//            log.add("ERROR: " + e.getMessage());
//        }
    }

    public void Close()
    {/*
     * before closing the file descriptor ,
     * write byteArray collection to the file descriptor
     * this has been done to increase the speed to generate a .bin file.
     * This has been done only for the BIN file.
     */
        try{
            byte[] temp = binOut.toByteArray();
            file_output.write(temp,0, ImageLength);
            file_output.flush();
            file_output.close();
        }
        catch (IOException e) {
            System.out.println ("IO exception = " + e );
            log.add("ERROR: " + e.getMessage());
        }
    }
  }

  /*
   * ASM file generator
   */
public class MPFS2ASM30Writer extends MPFS2Writer
{
public FileOutputStream file_output;
public DataOutputStream data_out;
public BufferedWriter fout;

public MPFS2ASM30Writer(String filename)
{
    Date date = new Date();
    SimpleDateFormat sdf = new SimpleDateFormat("E MMM dd yyyy hh:mm:ss");
    if (!filename.endsWith(".s"))
        filename += ".s";

    try{
    // Create an output stream to the file.
    file_output = new FileOutputStream (filename);
    // Wrap the FileOutputStream with a DataOutputStream
    data_out = new DataOutputStream (file_output);
    fout = new BufferedWriter(new OutputStreamWriter(data_out));

    imageName = filename;
    fout.write(
        ";*****************************************************************************\r\n" +
        ";  AUTO-GENERATED CODE:  Microchip MPFS Generator version: 2.2.2\r\n" +
        ";\r\n" +
        ";  Microchip TCP/IP Stack Application Demo\r\n" +
        ";\r\n" +
        ";  Company:\r\n" +
        ";    Microchip Technology Inc.\r\n" +
        ";\r\n" +
        ";  File Name:\r\n" +
        ";    mpfs_img2.s\r\n" +
        ";\r\n" +
        ";  Summary:\r\n" +
        ";    NOT FOR HAND MODIFICATION.\r\n" +
        ";    This file is automatically generated by the MPFS Generator Utility.\r\n" +
        ";    ALL MODIFICATIONS WILL BE OVERWRITTEN BY THE MPFS GENERATOR.\r\n" +
        ";    Generated " + sdf.format(date) + " " + "\r\n" +
        ";\r\n" +
        ";  Description:\r\n" +
        ";    Defines an MPFS2 image to be stored in program memory.\r\n" +
        ";    Defined in ASM30 assembly for optimal storage size.\r\n" +
        ";*****************************************************************************\r\n\r\n" +

        ";DOM-IGNORE-BEGIN\r\n" +
        ";*****************************************************************************\r\n" +
        ";Software License Agreement\r\n" +
        ";Copyright(c) 2014 Microchip Technology Inc. All rights reserved.\r\n" +
        ";Microchip licenses to you the right to use, modify, copy and distribute\r\n" +
        ";Software only when embedded on a Microchip microcontroller or digital\r\n" +
        ";signal controller that is integrated into your product or third party\r\n" +
        ";product (pursuant to the sublicense terms in the accompanying license\r\n" +
        ";agreement).\r\n" +
        ";\r\n" +
        ";You should refer to the license agreement accompanying this Software\r\n" +
        ";for additional information regarding your rights and obligations.\r\n" +
        ";\r\n" +
        ";SOFTWARE AND DOCUMENTATION ARE PROVIDED \"AS IS\" WITHOUT WARRANTY OF ANY\r\n" +
        ";KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY\r\n" +
        ";OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR\r\n" +
        ";PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR\r\n" +
        ";OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,\r\n" +
        ";BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT\r\n" +
        ";DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,\r\n" +
        ";INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,\r\n" +
        ";COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY\r\n" +
        ";CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),\r\n" +
        ";OR OTHER SIMILAR COSTS.\r\n" +
        ";*****************************************************************************\r\n" +
        ";DOM-IGNORE-END\r\n\r\n" +

        ".equ VALID_ID,0\r\n" +
        ";.ifdecl __dsPIC30F\r\n" +
        ";    .include \"p30fxxxx.inc\"\r\n" +
        ";.endif\r\n" +
        ";.ifdecl __dsPIC33F\r\n" +
        ";    .include \"p33fxxxx.inc\"\r\n" +
        ";.endif\r\n" +
        ";.ifdecl __dsPIC33E\r\n" +
        ";    .include \"p33exxxx.inc\"\r\n" +
        ";.endif\r\n" +
		";.ifdecl __PIC24E\r\n" +
        ";    .include \"p24exxxx.inc\"\r\n" +
        ";.endif\r\n" +
        ";.ifdecl __PIC24H\r\n" +
        ";    .include \"p24hxxxx.inc\"\r\n" +
        ";.endif\r\n" +
        ";.ifdecl __PIC24F\r\n" +
        ";    .include \"p24fxxxx.inc\"\r\n" +
        ";.endif\r\n" +
        ".include \"xc.inc\"\r\n" +
        ".if VALID_ID <> 1\r\n" +
        "    .error \"Processor ID not specified in generic include files.  New ASM30 assembler needs to be downloaded?\"\r\n" +
        ".endif\r\n" +
        "	.text\r\n" +
        "	.section	MPFSData,code\r\n\r\n" +
        "	goto END_OF_MPFS	; Prevent accidental execution of constant data.\r\n" +
        "	.global BEGIN_MPFS\r\n" +
        "BEGIN_MPFS:"
    );
    }catch (IOException e) {
        System.out.println ("IO exception = " + e );
        log.add("ERROR: " + e.getMessage());
    }
}

public void Write(byte data)
{
    try{
    String tempStr;
    if (ImageLength % 12 == 0)
        fout.write("\r\n\t.pbyte\t");
    tempStr = String.format("%02x", data);
    fout.write("0x" + tempStr);
    ImageLength++;
    if(ImageLength % 12 != 0)
        fout.write(",");
    }catch (IOException e) {
        System.out.println ("IO exception = " + e );
        log.add("ERROR: " + e.getMessage());
    }
}

public void Close()
{
    try{
    if (ImageLength % 12 == 0)
        fout.write(",");
    fout.write(
        "0x00\r\n" +
        "END_OF_MPFS:\r\n\r\n" +
        "	.section	.const,psv\r\n" +
        "	.global	_MPFS_Start\r\n" +
        "_MPFS_Start:\r\n" +
        "	.long	paddr(BEGIN_MPFS)\r\n\r\n" +
        "	.section	MPFSHelpers,code\r\n\r\n" +
        "	.global _ReadProgramMemory\r\n" +
        "_ReadProgramMemory:\r\n" +
        "	push		_TBLPAG\r\n" +
        "	mov			w1,_TBLPAG\r\n" +
        "	mov			w0,w5\r\n" +
        "	tblrdl		[w5],w0\r\n" +
        "	tblrdh		[w5],w1\r\n" +
        "	pop			_TBLPAG\r\n" +
        "	return\r\n"
    );
    fout.flush();
    fout.close();
    } catch (IOException e) {
        System.out.println ("IO exception = " + e );
        log.add("ERROR: " + e.getMessage());
    }
 }
}
private class FilesRecordWriter 
{
public FileOutputStream file_output;
public DataOutputStream data_out;
public BufferedWriter fout;
public int ImageLength = 0;
    public FilesRecordWriter(String dataPath)
    {
       String filename = "FileRcrd.bin";

      // String newPath = dataPath+File.separator+filename;
       String newPath = dataPath+filename;
       try{
        // Create an output stream to the file.
        file_output = new FileOutputStream (newPath);
       }catch(IOException e)
       {
         log.add("ERROR: " + e.getMessage());
       }
    }

    public void Write(byte data)
    {
        try{
        //fout.write(data);
        file_output.write(data);
        ImageLength++;
        }catch(IOException e)
        {
          log.add("ERROR: " + e.getMessage());
        }
    }

    public void Close()
    {
        try{
         file_output.flush();
         file_output.close();
        }catch(IOException e)
        {
          log.add("ERROR: " + e.getMessage());
        }
    }
}

private class DynamicVarRecordWriter 
{
public FileOutputStream file_output;
public DataOutputStream data_out;
public BufferedWriter fout;

    public DynamicVarRecordWriter(String dataPath)
    {

        String filename = "DynRcrd.bin";

         //Create a new subfolder under the current active folder
       String newPath = dataPath+filename;
       try{
        // Create an output stream to the file.
        file_output = new FileOutputStream (newPath);
       }catch(IOException e)
       {
        log.add("ERROR: " + e.getMessage());
       }
    }

    //public override void Write(byte data)
    public void Write(byte data)
    {
     try{
      file_output.write(data);
     }catch(IOException e)
     {
      log.add("ERROR: " + e.getMessage());
     }
    }

    //public override void Close()
     public void Close()
     {
       try{
           file_output.flush();
           file_output.close();
       }catch(IOException e)
       {
        log.add("ERROR: " + e.getMessage());
       }
     }
}

//public bool MDDWriter(String localPath)
public void MDDWriter(String localPath)
{
    FilesRecordWriter FileRecrd /*Files with dynamic variables Record*/;
    DynamicVarRecordWriter DynVarRecrd /*Dynamic Variables Record of each file*/;

    int counter = 0;
    int loopCntr = 0;
    int numFileRecrds = 0;

    FileRecrd = new FilesRecordWriter(localPath);
    DynVarRecrd = new DynamicVarRecordWriter(localPath);

    int flags;
    List<FileRecord> FileRcrdList = new ArrayList<FileRecord>();
//FileRcrdList.
    for(MPFSFileRecord file : files)
    {
      counter=0;
      loopCntr=0;
      if(file.dynVarCntr >0)
      {
        FileRcrdList.add(new FileRecord((short)file.nameHash,
                    (int)file.fileRecordOffset,(int)file.dynVarCntr));
        numFileRecrds++;
        DynVarRecrd.Write((byte)(file.fileRecordLength));
        DynVarRecrd.Write((byte)(file.fileRecordLength>>8));
        DynVarRecrd.Write((byte)(file.fileRecordLength>>16));
        DynVarRecrd.Write((byte)(file.fileRecordLength>>24));

        flags = 0;
        if (file.hasIndex)
        flags |= MPFS2_FLAG_HASINDEX;
        if (file.isZipped)
        flags |= MPFS2_FLAG_ISZIPPED;

        DynVarRecrd.Write((byte)(flags));
        DynVarRecrd.Write((byte)(flags>>8));

        loopCntr=0;
        while(loopCntr!=file.dynVarCntr)
        {
          DynVarRecrd.Write((byte)(file.dynVarOffsetAndIndexID.get(0+counter)));
          DynVarRecrd.Write((byte)(file.dynVarOffsetAndIndexID.get(1+counter)));
          DynVarRecrd.Write((byte)(file.dynVarOffsetAndIndexID.get(2+counter)));
          DynVarRecrd.Write((byte)(file.dynVarOffsetAndIndexID.get(3+counter)));

          DynVarRecrd.Write((byte)(file.dynVarOffsetAndIndexID.get(4+counter)));
          DynVarRecrd.Write((byte)(file.dynVarOffsetAndIndexID.get(5+counter)));
          DynVarRecrd.Write((byte)(file.dynVarOffsetAndIndexID.get(6+counter)));
          DynVarRecrd.Write((byte)(file.dynVarOffsetAndIndexID.get(7+counter)));

          counter+=8;
          loopCntr+=1;
        }
      }
    }

    FileRecrd.Write((byte)(numFileRecrds));
    FileRecrd.Write((byte)(numFileRecrds>>8));
    FileRecrd.Write((byte)(numFileRecrds>>16));
    FileRecrd.Write((byte)(numFileRecrds>>24));

    //FileRcrdList.ForEach(delegate(FileRecord FR)
    List<FileRecord> tempfileRecrdList = FileRcrdList;
    Collections.sort(tempfileRecrdList);
    
    for(FileRecord FR : FileRcrdList)    
    {
      FileRecrd.Write((byte)(FR.nameHash));
      FileRecrd.Write((byte)(FR.nameHash>>8));
      FileRecrd.Write((byte)(FR.fileRecordOffset));
      FileRecrd.Write((byte)(FR.fileRecordOffset>>8));
      FileRecrd.Write((byte)(FR.fileRecordOffset>>16));
      FileRecrd.Write((byte)(FR.fileRecordOffset>>24));
      FileRecrd.Write((byte)(FR.dynVarCntr));
      FileRecrd.Write((byte)(FR.dynVarCntr>>8));
      FileRecrd.Write((byte)(FR.dynVarCntr>>16));
      FileRecrd.Write((byte)(FR.dynVarCntr>>24));
    }
  
    log.add("\r\nGENERATED MPFS2 IMAGE: " + FileRecrd.ImageLength + " bytes");
    FileRecrd.Close();
    DynVarRecrd.Close();

}
public class MPFSClassicBuilder //extends MPFSBuilder
{
    public int ReserveBlock;
    Date date = new Date();
    SimpleDateFormat sdf = new SimpleDateFormat("E MMM dd yyyy hh:mm:ss");
    private FileOutputStream file_output;
    private DataOutputStream data_out;
    private BufferedWriter fout;
    /// <summary>
    /// Creates a new MPFS Classic image builder
    /// </summary>
    /// <param name="localPath">The directory to save the image in</param>
    /// <param name="localFile">The output file name for the image</param>
    public MPFSClassicBuilder(String localPath, String localFile)
    {
        if (!localPath.endsWith("\\"))
            localPath += File.separator;
        LocalPath = localPath;
        LocalFile = localFile;
        ReserveBlock = 0;
        log = new ArrayList<String>();
        files = new LinkedList<MPFSFileRecord>();
        indexUpdated = false;
    }

    byte MPFS_DATA = 0x00;
    byte MPFS_DELETED = 0x01;
    byte MPFS_DLE = 0x03;
    byte MPFS_ETX = 0x04;

    String MPFS_C_HEADER =
        "/*****************************************************************************\r\n" +
        "  AUTO-GENERATED CODE:  Microchip MPFS Generator version: 2.2.2\r\n" +
        "\r\n" +
        "  Microchip TCP/IP Stack Application Demo\r\n" +
        "\r\n" +
        "  Company:\r\n" +
        "    Microchip Technology Inc.\r\n" +
        "\r\n" +
        "  File Name:\r\n" +
        "    mpfs_img.c\r\n" +
        "\r\n" +
        "  Summary:\r\n" +
        "    NOT FOR HAND MODIFICATION.\r\n" +
        "    This file is automatically generated by the MPFS Generator Utility.\r\n" +
        "    ALL MODIFICATIONS WILL BE OVERWRITTEN BY THE MPFS GENERATOR.\r\n" +
        "    Generated " + sdf.format(date) + " " + "\r\n" +
        "\r\n" +
        "  Description:\r\n" +
        "    Defines an MPFS2 image to be stored in program memory.\r\n" +
        " *****************************************************************************/\r\n\r\n" +

       "#define __MPFSIMG_C_\r\n" +
        "\r\n" +
        "#include \"tcpip/tcpip.h\"\r\n" +
        "\r\n" +
        "#if defined(STACK_USE_MPFS) && !defined(MPFS_USE_EEPROM) && !defined(MPFS_USE_SPI_FLASH)\r\n" +
        "\r\n";
    String MPFS_C_FOOTER =
        "/**************************************************************\r\n" +
        " * End of MPFS\r\n" +
        " **************************************************************/\r\n" +
        "#endif // #if defined(STACK_USE_MPFS) && !defined(MPFS_USE_EEPROM) && !defined(MPFS_USE_SPI_FLASH)" +
        "\r\n\r\n";
    String MPFS_ASM_HEADER =
        ";*****************************************************************************\r\n" +
        ";  AUTO-GENERATED CODE:  Microchip MPFS Generator version: 2.2.2\r\n" +
        ";\r\n" +
        ";  Microchip TCP/IP Stack Application Demo\r\n" +
        ";\r\n" +
        ";  Company:\r\n" +
        ";    Microchip Technology Inc.\r\n" +
        ";\r\n" +
        ";  File Name:\r\n" +
        ";    mpfs_img.s\r\n" +
        ";\r\n" +
        ";  Summary:\r\n" +
        ";    NOT FOR HAND MODIFICATION.\r\n" +
        ";    This file is automatically generated by the MPFS Generator Utility.\r\n" +
        ";    ALL MODIFICATIONS WILL BE OVERWRITTEN BY THE MPFS GENERATOR.\r\n" +
        ";    Generated " + sdf.format(date) + " " + "\r\n" +
        ";\r\n" +
        ";  Description:\r\n" +
        ";    Defines an MPFS2 image to be stored in program memory.\r\n" +
        ";    Defined in ASM30 assembly for optimal storage size.\r\n" +
        ";*****************************************************************************\r\n\r\n" +

        ".equ VALID_ID,0\r\n" +
        ";.ifdecl __dsPIC30F\r\n" +
        ";    .include \"p30fxxxx.inc\"\r\n" +
        ";.endif\r\n" +
        ";.ifdecl __dsPIC33F\r\n" +
        ";    .include \"p33fxxxx.inc\"\r\n" +
        ";.endif\r\n" +
        ";.ifdecl __PIC24H\r\n" +
        ";    .include \"p24hxxxx.inc\"\r\n" +
        ";.endif\r\n" +
        ";.ifdecl __PIC24F\r\n" +
        ";    .include \"p24fxxxx.inc\"\r\n" +
        ";.endif\r\n" +
        ".include \"xc.inc\"\r\n" +
        ".if VALID_ID <> 1\r\n" +
        "    .error \"Processor ID not specified in generic include files.  New ASM30 assembler needs to be downloaded?\"\r\n" +
        ".endif\r\n" +
        "	 .text\r\n" +
        "	 .section	MPFSData,code\r\n\r\n" +
        "	 goto END_OF_MPFS	; Prevent accidental execution of constant data.\r\n" +
        "	 .global BEGIN_MPFS\r\n" +
        "BEGIN_MPFS:\r\n";
    String MPFS_ASM_FOOTER = "\r\n\r\n; End of Generated Image\r\n";

    /// <summary>
    /// Adds a file to the MPFS image
    /// </summary>
    /// <param name="localName">Local file name to read</param>
    /// <param name="imageName">Name to use in image file</param>
    public boolean AddFile(String localName, String imageName)
    {
        // Skip if can't be opened
        if (!new File(localName).exists())
        {
            log.add("\r\nERROR: Could not read " + localName);
            return false;
        }

        // Set up the new file record
        MPFSFileRecord newFile = new MPFSFileRecord();
        newFile.SetFileName(imageName);

        // Read the data in, escaping as it is read
        byte b;
        int byteRead;

        try{
        ByteArrayOutputStream tempout = new ByteArrayOutputStream();
        FileInputStream inputFile = new FileInputStream(localName);

        while((byteRead=inputFile.read()) != -1){
            if (byteRead == MPFS_DLE || byteRead == MPFS_ETX)
                tempout.write((byte)MPFS_DLE);
            tempout.write((byte)byteRead);
        }
        newFile.data = tempout.toByteArray();
        newFile.fileSizeLen =  tempout.size();
        inputFile.close();
        // Add the file and return
        log.add("    " + imageName + ": " + newFile.fileSizeLen + " bytes");
        files.add(newFile);
        }catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }

        return true;
    }

    /// <summary>
    /// Adds a directory to the MPFS image.  All non-hidden files will be included.
    /// </summary>
    /// <param name="dataPath">The local directory to search</param>
    /// <param name="imagePath">Ignored for MPFS Classic</param>
    /// <returns></returns>
    public boolean AddDirectory(String dataPath, String imagePath)
    {
        File dir = new File(dataPath);
        // Make sure directory exists
        if (!dir.exists())
        {
            log.add("\r\nERROR: Directory " + dataPath + " does not exist.");
            return false;
        }

        // Make sure directory is not the project directory
        if (LocalPath.contains(dataPath))
        {
            log.add("\r\nERROR: The project directory is located in the source " +
                    "directory.\r\n The generator cannot run if the image is" +
                    " \r\nto be placed " +
                    "in the source directory." +
                    "\r\n  Please select the base MPLAB project " +
                    "\r\ndirectory before continuing.");
            return false;
        }

        // Load directory members
        //DirectoryInfo dir = new DirectoryInfo(dataPath);
        File[] filelist = dir.listFiles();

        log.add(dataPath + " :");
        for(File f : filelist)
        {
            if(f.isHidden()||f.isDirectory())
                continue;
            else if(f.isFile())
            {
                this.AddFile(f.getPath(), imagePath+f.getName());
            }
        }

        return true;
    }

    public boolean Generate(MPFSOutputFormat format)
    {
        // Start with nothing
        generatedImageName = null;
        File dir = new File(LocalPath);
        // Make sure all paths exist
        if (!dir.exists())
        {
            log.add("\r\nERROR: Output directory \"" + LocalPath + "\" does not exist!");
            return false;
        }

        // Make sure we have some files
        if(files.size() == 0)
            return false;

        // Generate based on format
        try
        {
            switch (format)
            {
                case BIN:
                    return GenerateBIN(LocalPath + LocalFile);
                case C18:
                case C32:
                    return GenerateC(LocalPath + LocalFile);
                case ASM30:
                    return GenerateASM(LocalPath + LocalFile);
                default:
                    log.add("\r\nERROR: Invalid output format was specified.");
                    return false;
            }
        }
        catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }

    }

    private boolean GenerateBIN(String filename)
    {
       ByteArrayOutputStream file_output = new ByteArrayOutputStream();
        // Open the file
        if (!filename.endsWith(".bin"))
            filename += ".bin";
        try{
        FileOutputStream classicbinOut = new FileOutputStream (filename);
      
        // Write the FAT data
        int baseAddr = ReserveBlock + 17 * ((int)files.size() + 1);
        for(MPFSFileRecord file : files)
        {
            file_output.write(MPFS_DATA);
            file_output.write((byte)(baseAddr));
            file_output.write((byte)(baseAddr >> 8));
            file_output.write((byte)(baseAddr >> 16));
            file_output.write((byte)(baseAddr >> 24));
            file_output.write(NormalizeFileName(file.GetFileName()).getBytes());
            //baseAddr += (int)file.data.size() + 5;
            baseAddr += (int)file.fileSizeLen+5;
        }
        file_output.write(MPFS_ETX);
        file_output.write(0xff);
        file_output.write(0xff);
        file_output.write(0xff);
        file_output.write(0xff);
        file_output.write("END OF FAT  ".getBytes());

        // Write the files
        for(MPFSFileRecord file : files)
        {
             for(int i=0;i<file.fileSizeLen;i++)
                file_output.write(file.data[i]);
            file_output.write(MPFS_ETX);
            file_output.write(0xff);
            file_output.write(0xff);
            file_output.write(0xff);
            file_output.write(0xff);
        }
        int fileSize = file_output.size();
        classicbinOut.write(file_output.toByteArray(),0,fileSize);
        // Flush the output and store the file name
        classicbinOut.flush();
        classicbinOut.close();
        }catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }
        generatedImageName = filename;
        return true;
    }

    private boolean GenerateC(String filename)
    {
        String tempStr;
        // Open the file
        if (!filename.endsWith(".c"))
            filename += ".c";
        try{
        file_output = new FileOutputStream (filename);
        // Wrap the FileOutputStream with a DataOutputStream
        data_out = new DataOutputStream (file_output);
        fout = new BufferedWriter(new OutputStreamWriter(data_out));

        fout.write(MPFS_C_HEADER);

        // Write the files
        int fileIndex = 0;
        for(MPFSFileRecord file : files)
        {
            fout.write("\r\n/*******************************\r\n * Begin ");
            fout.write(NormalizeFileName(file.GetFileName()));
            fout.write("\r\n *******************************/\r\n\r\nstatic ROM unsigned char MPFS_");
            tempStr = String.format("%04x",fileIndex++)+"[] = \r\n{";
            fout.write(tempStr);

            for (int i = 0; i < file.fileSizeLen; i++)
            {
                if (i % 12 == 0)
                    fout.write("\r\n\t");
                tempStr = String.format("0x%02x",file.data[i])+",";
                fout.write(tempStr);
            }

            fout.write("\r\n\t0x04,0xff,0xff,0xff\r\n};\r\n");
        }

        // Write the FAT
        fileIndex = 0;
        fout.write(
            "/**************************************************\r\n" +
            " * Start of MPFS FAT\r\n" +
            " **************************************************/\r\n" +
            "typedef struct\r\n" +
            "{\r\n" +
            "    unsigned char Flags;\r\n" +
            "    ROM unsigned char* Address;\r\n" +
            "    unsigned char Name[12];\r\n" +
            "} FAT_TABLE_ENTRY;\r\n" +
            "\r\n" +
            "ROM FAT_TABLE_ENTRY MPFS_Start[] = \r\n" +
            "{"
        );
        for(MPFSFileRecord file : files)
        {
            tempStr = "\r\n\t{ 0x00, MPFS_"+String.format("%04x",fileIndex++);
            fout.write(tempStr);
            for(byte b : NormalizeFileName(file.GetFileName()).getBytes())
            {
                fout.write(", '" + (char)b + "'");
            }
            fout.write(" },");
        }
        fout.write(
            "\r\n\t{ 0x04, (ROM unsigned char*)0xffffff, 'E', 'N', 'D', ' ', 'O', 'F', ' ', 'F', 'A', 'T', ' ', ' ' },\r\n" +
            "};\r\n" +
            "/**************************************************\r\n" +
            " * End of MPFS FAT\r\n" +
            " **************************************************/\r\n\r\n"
        );

        fout.write(MPFS_C_FOOTER);

        // Flush the output and store the file name
        fout.flush();
        fout.close();
        }catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }
        generatedImageName = filename;
        return true;
    }

    private boolean GenerateASM(String filename)
    {
        String tempStr;
        // Open the file
        if (!filename.endsWith(".s"))
            filename += ".s";
        try{
        file_output = new FileOutputStream (filename);
        // Wrap the FileOutputStream with a DataOutputStream
        data_out = new DataOutputStream (file_output);
        fout = new BufferedWriter(new OutputStreamWriter(data_out));

        fout.write(MPFS_ASM_HEADER);

        // Write the files
        int fileIndex = 0;
        for(MPFSFileRecord file : files)
        {
            fout.write("\r\n;*******************************\r\n;  Begin ");
            fout.write(NormalizeFileName(file.GetFileName()));
            fout.write("\r\n;*******************************\r\n" +
                "\tgoto\tEND_OF_MPFS_");
            tempStr = String.format("%04x",fileIndex);
            fout.write(tempStr);
            fout.write("\t\t; Prevent accidental execution of constant data\r\n\t.global _MPFS_");
            fout.write(tempStr);
            fout.write("\r\n_MPFS_");
            fout.write(tempStr);
            fout.write(":");

            for (int i = 0; i < file.fileSizeLen; i++)
            {
                if (i % 12 == 0)
                    fout.write("\r\n\t.pbyte\t");
                tempStr = String.format("0x%02x",file.data[i]);
                fout.write(tempStr);
                if (i % 12 != 11 && i != file.fileSizeLen-1)
                    fout.write(",");
            }

            fout.write("\r\n\t.pbyte\t0x04,0xff,0xff,0xff,0xff\r\nEND_OF_MPFS_");
            tempStr = String.format("%04x",fileIndex++);
            fout.write(tempStr);
            fout.write(":\r\n");
        }

        // Write the FAT
        fileIndex = 0;
        fout.write(
            ";*************************************************\r\n" +
            ";  Start of MPFS FAT\r\n" +
            ";*************************************************\r\n" +
            "\t.section\t.const,psv\r\n" +
            "\t.global _MPFS_Start\r\n" +
            "_MPFS_Start:"
        );
        for(MPFSFileRecord file : files)
        {
            fout.write("\r\n\t.byte\t0x00,0x00\r\n\t.long\tpaddr(_MPFS_");
            tempStr = String.format("%04x",fileIndex++);
            fout.write(tempStr);
            fout.write(")\r\n\t.byte\t");
            int i = 0;
            for(byte b : NormalizeFileName(file.GetFileName()).getBytes())
            {
                fout.write("'" + (char)b + "'");
                if (++i < 12)
                    fout.write(",");
            }
        }
        fout.write(
            "\r\n\t.byte\t0x04,0x00\r\n\t.long\t0xffffffff" +
            "\r\n\t.byte\t'E','N','D',' ','O','F',' ','F','A','T',' ',' '\r\n\r\n" +
            "\t.section MPFSEnd,code\r\n" +
            "END_OF_MPFS:\r\n"
        );

        fout.write(MPFS_ASM_FOOTER);

        // Flush the output and store the file name
        fout.flush();
        fout.close();
        }catch (Exception e)
        {
            log.add("\r\nERROR: " + e.getMessage());
            return false;
        }
        generatedImageName = filename;
        return true;
    }

    private String NormalizeFileName(String name)
    {
        if (name.length() > 12)
            name = name.substring(0, 12);
        return padRight(name,12).toUpperCase();
    }
}

public static String padRight(String s, int n) {
     return String.format("%1$-" + n + "s", s);
}

public static String padLeft(String s, int n) {
    return String.format("%1$" + n + "s", s);
}

}
