using Avalonia;
using Avalonia.Markup.Xaml;

namespace Editor_App
{
    public class App : Application
    {
        public override void Initialize()
        {
            AvaloniaXamlLoader.Load(this);
        }
   }
}