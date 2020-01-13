
/***                    DataToString.class
****    return the supplied long time in Country Specific Format
***/


import java.util.*;
import java.text.*;

public class DateToString
{
	//private static DateToString dateToString = new DateToString ("GMT", Locale.UK);
	//private DateFormat dateFormat;

    private DateToString (String timezone, Locale loc)
    {
      /*  TimeZone tz = TimeZone.getTimeZone(timezone);
        tz.setDefault(tz);
        dateFormat = DateFormat.getDateTimeInstance (DateFormat.MEDIUM, DateFormat.LONG,loc);
        dateFormat.setTimeZone(tz);*/

	}

    public static String getString (long time)
    {
    	return "20 Feb 2007 09:20:50 GMT";
		//return dateToString.dateFormat.format (new Date (time));
    }

}