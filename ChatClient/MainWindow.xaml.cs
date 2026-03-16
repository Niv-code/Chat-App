using System.Windows;

namespace ChatClient
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            // חיבור המסך (View) למוח שלו (ViewModel)
            DataContext = new ChatViewModel();
        }
    }
}