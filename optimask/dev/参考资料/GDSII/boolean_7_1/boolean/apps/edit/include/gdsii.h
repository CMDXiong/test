/*
Program	GDSII.H
Purpose	Definition of GDSII structure (Header)
Programmer	N. Noorlander & R. Spekreijse
Last Update	12-12-1995
*/

#ifndef GDSII_H
#define GDSII_H

#ifdef __GNUG__
#pragma interface
#endif

#include "misc.h"
#include "bbox.h"
#include "gdserr.h"
#include "referenc.h"
#include "elemlst.h"
#include "group.h"
#include "proper.h"

class Structure;

class ModeTime
{
	public:
		ModeTime();
		int	GetLmtYear();
		int	GetLmtMonth();
		int	GetLmtDay();
		int	GetLmtHour();
		int	GetLmtMinute();
		int	GetLmtSecond();
		int	GetLatYear();
		int	GetLatMonth();
		int	GetLatDay();
		int	GetLatHour();
		int	GetLatMinute();
		int	GetLatSecond();
		void	SetLmtYear(int);
		void	SetLmtMonth(int);
		void	SetLmtDay(int);
		void	SetLmtHour(int);
		void	SetLmtMinute(int);
		void	SetLmtSecond(int);
		void	SetLatYear(int);
		void	SetLatMonth(int);
		void	SetLatDay(int);
		void	SetLatHour(int);
		void	SetLatMinute(int);
		void	SetLatSecond(int);
		ModeTime&	operator=(ModeTime& other);

	protected:
		// lmt = last modification time
		int 	_lmt_year;
		int 	_lmt_month;
		int 	_lmt_day;
		int 	_lmt_hour;
		int 	_lmt_minute;
		int 	_lmt_second;

		// lat = last access time
		int 	_lat_year;
		int 	_lat_month;
		int 	_lat_day;
		int 	_lat_hour;
		int 	_lat_minute;
		int 	_lat_second;
};


class Library
{
	public:
		// contructor & destructor
		Library();
		~Library();

		// public memberfunction
		bool					FileOpen();

		// get and set functions
		void				SetFilename(const wxString&);
		void				SetFontsname(const wxString&);

		wxString       GetFilename();
		wxString       GetFontsname();

		void				SetVersion(int nr);
		int				GetVersion();

		wxString       GetLibraryName();
		void				SetLibraryName(const wxString&);

		// assignment operator
		Library& 				operator=(Library&);

		double GetUserUnits();
		double GetPhysicalUnits();
		void	 SetUserUnits(double);
		void	 SetPhysicalUnits(double);

	protected:
		double		  _userunits;
		double		  _physunits;
		bool			  _fileopened;
		int			  _filehandle;
		int 			  _version;

		wxString 	  _filename;
		wxString		  _fontsname;
		wxString		  _libraryname;
};

class DrawDriver;
class Driver_out;

class GdsII :  public DL_List<void*>
{
	public:
		// constructors and destructors
		GdsII();
		~GdsII();
      void Write(DrawDriver& dr);

		double GetUserUnits();
		double GetPhysicalUnits();
		void	 SetUserUnits(double);
		void	 SetPhysicalUnits(double);
		int    GetVersion();
		void	 SetVersion(int);

		//bool Recurse(wxPropertyValues* args);
		void 	 Operation(OPS operation,EleFlags maskA,EleFlags maskB,int destination,bool m_destination_add);
      void	 SetGroupFlags(group *a_group,EleFlags mask);
      void	 SetGroupFlagsRefs(EleFlags mask,bool srefselected);
      void	 ClearFlags(short int mask);
      void   SetFlags(EleFlags mask,bool selectedonly);
      void   SetAvailable();
		void   Flatten(bool selected_only,bool deep);
      void   Transform(bool mirror_x,bool mirror_y,double move_h,double move_v,
      						double scale_h_ratio,double scale_v_ratio,double degr,
                        EleFlags mask);
      void   UndoTransform(EleFlags mask);
      void   Explode(bool selectedonly);
		void   CalcArea(double* area,EleFlags mask);
		bool   AddProperty(const wxString& name,const wxString& type,const wxString& value,bool _selectedonly);
		bool 	 AddProperty(EleFlags mask,const wxString& name,const wxString& type,const wxString& value);
		bool 	 Move(int destination,EleFlags mask);
		bool 	 Copy(int destination,EleFlags mask);
		bool 	 ToArcs(int destination,EleFlags mask);
		bool 	 ToLines(int destination,EleFlags mask);
		bool 	 Delete(EleFlags mask);
		bool   Delete_A(EleFlags mask);


		// get and set memberfunctions
		int						LinkReferences();
		BoundingBox&			CalcBoundingBox();
		BoundingBox&			GetBoundingBox();
		ModeTime*				GetBgnlib();
		Library*				   GetLibrary();
		Structure*				GetTopStructure();
		void						SetBgnlib(ModeTime*);
		void						SetLibrary(Library*);
		void						SetTopStructure(Structure*);
		void						ShowInformation();
		void						MakeStructure(wxString& Structure);

		Structure*	GetStructure(const wxString&);
		Structure*	SearchTopStructure();
		void 			ResetReferencesNr();
		void 			ResetBinHere();
		void 			ResetExplodeReferencesNr();
		void			CheckEmpty(Structure* the_top);
		void 			SortOnBoundingBox();
      void			DeleteBin();


		// Operations on GDS data
		void  							SetReferenceNr();

		// operator overloading
		GdsII&							operator=(GdsII&);
		Structure*			       	GetShowStructure();
		void						      SetShowStructure(Structure*);
		void 								Merge(GdsII* other_gdsii,double x,double y,double angle);
      void                       SetModTime();
      void                       ResetUnits(double);
		void 								Replace(const wxString& to_replace,GdsII* other_gdsii);
      bool                       GetMultiple(){return m_multipletop;};

	protected:
		int								_version;
		Structure*						_topstructure;
		Structure*						_showstructure;
		Library*							_library;
		ModeTime*						_bgnlib;
		BoundingBox						_boundingbox;
      wxTransformMatrix*			_trans_matrix;
      bool                       m_multipletop;
};


#endif

