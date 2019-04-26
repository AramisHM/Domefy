using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ConvertImages
{
    public partial class Form1 : Form
    {
        string[] files;
        bool dirSelected = false;
        public Form1()
        {
            InitializeComponent();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            using (var fbd = new FolderBrowserDialog())
            {
                DialogResult result = fbd.ShowDialog();

                if (result == DialogResult.OK && !string.IsNullOrWhiteSpace(fbd.SelectedPath))
                {
                    files = Directory.GetFiles(fbd.SelectedPath);
                    textBox3.Text = fbd.SelectedPath.ToString();

                    System.Windows.Forms.MessageBox.Show("Files found: " + files.Length.ToString(), "Message");
                    dirSelected = true;
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(dirSelected == true)
            {
                string targetDir = textBox3.Text + "\\materials\\";
                if (Directory.Exists(targetDir))
                    Directory.Delete(targetDir);

                Directory.CreateDirectory(targetDir);

                foreach (string file in files)
                {
                    // resize the image
                    try
                    {

                        string s = "<material>" +
	                    "<technique name=\"Techniques/DiffAdd.xml\" />" +
                        "<texture unit=\"diffuse\" name=\"./converted_images/" + Path.GetFileName(file) + "\" />" +
	                    "<parameter name=\"MatDiffColor\" value=\"1 1 1 1\" />" +
                    	"<depthbias constant=\"-0.00001\" slopescaled=\"0\" />" +
                        "</material>";

                        string filePath = targetDir + Path.GetFileNameWithoutExtension(file) + ".xml";
                        using (System.IO.StreamWriter fileA =
                         new System.IO.StreamWriter(filePath, true))
                        {
                            fileA.WriteLine(s);
                        }
                    }
                    catch
                    {
                        Console.WriteLine("XML could not be generated");
                    }
                }
            }
        }
    }
}
