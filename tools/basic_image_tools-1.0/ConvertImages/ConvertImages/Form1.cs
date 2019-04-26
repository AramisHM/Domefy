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

                        Bitmap b = new Bitmap(Int32.Parse(textBox1.Text), Int32.Parse(textBox2.Text));
                        using (Graphics g = Graphics.FromImage((Image)b))
                        {
                            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
                            g.DrawImage(oringImg, 0, 0, Int32.Parse(textBox1.Text), Int32.Parse(textBox2.Text));
                        }

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
