#include <boxer/boxer.h>
#include <windows.h>

namespace boxer {

namespace {

UINT getIcon(Style style) {
   switch (style) {
      case Style::Info:
         return MB_ICONINFORMATION;
      case Style::Warning:
         return MB_ICONWARNING;
      case Style::Error:
         return MB_ICONERROR;
      case Style::Question:
         return MB_ICONQUESTION;
      default:
         return MB_ICONINFORMATION;
   }
}

UINT getButtons(Buttons buttons) {
   switch (buttons) {
      case Buttons::OK:
         return MB_OK;
      case Buttons::OKCancel:
         return MB_OKCANCEL;
      case Buttons::YesNo:
         return MB_YESNO;
      case Buttons::AbortRetryIgnore:
         return MB_ABORTRETRYIGNORE;
	  case Buttons::CancelTryContinue:
         return MB_CANCELTRYCONTINUE;
      default:
         return MB_OK;
   }
}

Selection getSelection(int response) {
   switch (response) {
      case IDOK:
         return Selection::OK;
      case IDCANCEL:
         return Selection::Cancel;
      case IDYES:
         return Selection::Yes;
      case IDNO:
         return Selection::No;
      case IDABORT:
         return Selection::Abort;
      case IDRETRY:
         return Selection::Retry;
      case IDTRYAGAIN:
         return Selection::Try;
      case IDCONTINUE:
         return Selection::Continue;
      default:
         return Selection::None;
   }
}

} // namespace

Selection show(const char* message, const char *title, Style style, Buttons buttons) {
   UINT flags = MB_TASKMODAL;

   flags |= getIcon(style);
   flags |= getButtons(buttons);

#ifdef _UNICODE
   wchar_t temp_msg[256] = { 0 };
   wchar_t temp_title[256] = { 0 };

   mbstowcs(temp_msg, message, 256);
   mbstowcs(temp_title, title, 256);

   return getSelection(MessageBox(nullptr, temp_msg, temp_title, flags));
#else
   return getSelection(MessageBox(nullptr, message, title, flags));
#endif
}

} // namespace boxer
