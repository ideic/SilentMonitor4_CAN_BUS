using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UWPApp.ViewModels
{
    public interface IViewContext
    {
        void PostCommand(Action promise);   
    }
}
