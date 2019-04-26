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
                string targetDir = textBox3.Text + "\\converted_images\\";
                if (Directory.Exists(targetDir))
                    Directory.Delete(targetDir);

                Directory.CreateDirectory(targetDir);

                foreach (string file in files)
                {
                    Bitmap oringImg = new Bitmap(file);

                    // resize the image
                    try
                    {
                        int x1 = Int32.Parse(textBox1.Text);
                        int x2 = Int32.Parse(textBox2.Text);
                        int x3 = Int32.Parse(textBox4.Text);
                        int x4 = Int32.Parse(textBox5.Text);
                        // second pair is about size and not position
                        x3 = x3 - x1;
                        x4 = x4 - x2;

                        Bitmap b = oringImg.Clone(new Rectangle(x1, x2, x3, x4), oringImg.PixelFormat);

                        b.Save(targetDir + Path.GetFileName(file), System.Drawing.Imaging.ImageFormat.Jpeg);
                        GC.Collect();
                        GC.WaitForPendingFinalizers();
                    }
                    catch
                    {
                        Console.WriteLine("Bitmap could not be resized");
                    }
                }
            }
        }
    }
}
