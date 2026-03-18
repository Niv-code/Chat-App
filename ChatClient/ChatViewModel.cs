using System;
using System.Collections.ObjectModel;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;

namespace ChatClient
{
    public class ChatViewModel : ObservableObject
    {
        private TcpClient _client;
        private NetworkStream _stream;
        private string _currentMessage;

        public ObservableCollection<string> Messages { get; set; }

        public string CurrentMessage
        {
            get => _currentMessage;
            set { _currentMessage = value; OnPropertyChanged(); }
        }

        public ICommand ConnectCommand { get; }
        public ICommand SendCommand { get; }

        public ChatViewModel()
        {
            Messages = new ObservableCollection<string>();
            ConnectCommand = new RelayCommand(async (o) => await ConnectToServer());
            SendCommand = new RelayCommand(async (o) => await SendMessage());
        }

        private async Task ConnectToServer()
        {
            try
            {
                _client = new TcpClient();
                await _client.ConnectAsync("127.0.0.1", 54321);
                _stream = _client.GetStream();

                Messages.Add("System: Connected to server!");

                _ = ListenForMessages();
            }
            catch (Exception ex)
            {
                Messages.Add($"System: Connection failed ({ex.Message})");
            }
        }

        private async Task ListenForMessages()
        {
            byte[] buffer = new byte[1024];
            try
            {
                while (_client.Connected)
                {
                    int bytesRead = await _stream.ReadAsync(buffer, 0, buffer.Length);
                    if (bytesRead > 0)
                    {
                        string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                        Application.Current.Dispatcher.Invoke(() => Messages.Add($"Server: {message}"));
                    }
                }
            }
            catch
            {
                Application.Current.Dispatcher.Invoke(() => Messages.Add("System: Disconnected from server."));
            }
        }

        private async Task SendMessage()
        {
            if (_stream != null && _client.Connected && !string.IsNullOrWhiteSpace(CurrentMessage))
            {
                byte[] data = Encoding.UTF8.GetBytes(CurrentMessage);
                await _stream.WriteAsync(data, 0, data.Length);

                Messages.Add($"Me: {CurrentMessage}");
                CurrentMessage = string.Empty; 
            }
        }
    }
}