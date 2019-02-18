using System;
using System.Collections.Generic;
using System.IO;

class Program
{
    static List<string> available = new List<string>( new string[] { "outputs", "inputs", "weights", "neuron", "setupWeights", "run", "train" } );
    static string[] lines;
    static string assembly;
    static string p;
    static string dest;
    static int section = 0;
    static void Main(string[] args)
    {
        if (args.Length!=1)
        {
            Console.WriteLine("Wrong number of arguments, expected: 1, received:"+args.Length);
            System.Environment.Exit(1);
        }
        p = args[0];
        dest = Path.GetDirectoryName(p) + "/" + Path.GetFileNameWithoutExtension(p) + "_build/";
        Directory.CreateDirectory(dest);
        dest += "source.s";
        assembly = "";
        string text = File.ReadAllText(p);
        lines = text.Split('\n');
        process();
        Console.WriteLine("Compiler exited");
    }
    static void process()
    {
        foreach (string line in lines)
        {
            try
            {
                assembly += "\r\n" + processStatement(line);
            }
            catch {
                Console.WriteLine("Error on line " + Array.IndexOf(lines,line));
            }
        }
        File.WriteAllText(dest, assembly);
        
    }
    static string processStatement(string statement)
    {

        if (statement.Length < 1) {
            return "";
        }
        if (section>2)
        {
            return "";
        }
        statement = statement.Replace(" ", "");
        
        if (statement.Contains("--data")) 
        {
            return File.ReadAllText("chunks/start.chunk");
        }
        if (statement.Contains("--math"))
        {
            section = 1;
            return File.ReadAllText("chunks/defaultFunctions.chunk");
        }
        if (statement.Contains("--runtime"))
        {
            section = 2;
            return File.ReadAllText("chunks/runtime.chunk");
        }
        if (statement.Contains("--end"))
        {
            section = 3;
            return File.ReadAllText("chunks/end.chunk");
        }
        statement = statement.Substring(0,statement.IndexOf(';'));
        string[] tokens = statement.Split('<');
        string identifier = tokens[0];
        string[] arguments = tokens[1].Split(',');
        if (!available.Contains(identifier))
        {
            return "";
        }

        if (identifier == "weights")
        {
            available.Remove("weights");
            return "weights real8 " + arguments[0] + " DUP(" + arguments[1] + ")";
        }
        if (identifier == "inputs")
        {
            available.Remove("inputs");
            string o = "inputs ";
            foreach (string arg in arguments)
            {
                o += "real8 " + arg + "\r\n";
            }
            return o;
        }
        if (identifier == "outputs")
        {
            available.Remove("outputs");
            string o = "outputs ";
            foreach (string arg in arguments)
            {
                o += "real8 " + arg + "\r\n";
            }
            return o;
        }
        if (identifier == "run")
        {
            if (available.Contains("inputs") || available.Contains("outputs") || available.Contains("weights"))
            {
                Console.WriteLine("Trying to run network without setting up properly");
                return "";
            }
            else {
                return "mov eax,"+arguments[0]+ "\r\ncall run\nprintf(\"Result: %f\\n\", output)";
            }
        }
        if (identifier == "train")
        {
            Console.WriteLine("train");
            return "mov edx, " + arguments[0] + "\ntraining:\nmov eax, 0\nmov ebx, 0\ncall train\nmov eax, 3\nmov ebx, 1\ncall train\ndec edx\ncmp edx, 0\njnz training";
        }
        if (identifier == "setupWeights")
        {
            return "call setupWeights";
        }

        return "";
    }
}