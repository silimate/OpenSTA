// OpenSTA, Static Timing Analyzer
// Copyright (c) 2022, Parallax Software, Inc.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <string>
#include <memory>

#include "MinMax.hh"
#include "Vector.hh"
#include "Transition.hh"
#include "LibertyClass.hh"
#include "TimingModel.hh"

namespace sta {

using std::string;

class Unit;
class Units;
class Report;
class Table;
class OutputCurrent;
class OutputCurrentWaveform;

typedef Vector<float> FloatSeq;
typedef Vector<FloatSeq*> FloatTable;
typedef Vector<OutputCurrentWaveform*> OutputCurrentWaveformSeq;

TableAxisVariable
stringTableAxisVariable(const char *variable);
const char *
tableVariableString(TableAxisVariable variable);
const Unit *
tableVariableUnit(TableAxisVariable variable,
		  const Units *units);

class GateTableModel : public GateTimingModel
{
public:
  GateTableModel(TableModel *delay_model,
		 TableModel *delay_sigma_models[EarlyLate::index_count],
		 TableModel *slew_model,
		 TableModel *slew_sigma_models[EarlyLate::index_count],
                 ReceiverModelPtr receiver_model,
                 OutputCurrent *output_current);
  virtual ~GateTableModel();
  virtual void gateDelay(const LibertyCell *cell,
			 const Pvt *pvt,
			 float in_slew,
			 float load_cap,
			 float related_out_cap,
			 bool pocv_enabled,
			 // Return values.
			 ArcDelay &gate_delay,
			 Slew &drvr_slew) const;
  virtual void reportGateDelay(const LibertyCell *cell,
			       const Pvt *pvt,
			       float in_slew,
			       float load_cap,
			       float related_out_cap,
			       bool pocv_enabled,
			       int digits,
			       string *result) const;
  virtual float driveResistance(const LibertyCell *cell,
				const Pvt *pvt) const;

  const TableModel *delayModel() const { return delay_model_; }
  const TableModel *slewModel() const { return slew_model_;  }
  // Check the axes before making the model.
  // Return true if the model axes are supported.
  static bool checkAxes(const TablePtr table);

protected:
  void maxCapSlew(const LibertyCell *cell,
		  float in_slew,
		  const Pvt *pvt,
		  float &slew,
		  float &cap) const;
  virtual void setIsScaled(bool is_scaled);
  float axisValue(TableAxisPtr axis,
		  float load_cap,
		  float in_slew,
		  float related_out_cap) const;
  float findValue(const LibertyLibrary *library,
		  const LibertyCell *cell,
		  const Pvt *pvt,
		  const TableModel *model,
		  float in_slew,
		  float load_cap,
		  float related_out_cap) const;
  void reportTableLookup(const char *result_name,
			 const LibertyLibrary *library,
			 const LibertyCell *cell,
			 const Pvt *pvt,
			 const TableModel *model,
			 float in_slew,
			 float load_cap,
			 float related_out_cap,
			 int digits,
			 string *result) const;
  void findAxisValues(const TableModel *model,
		      float in_slew,
		      float load_cap,
		      float related_out_cap,
		      // Return values.
		      float &axis_value1,
		      float &axis_value2,
		      float &axis_value3) const;
  static bool checkAxis(TableAxisPtr axis);

  TableModel *delay_model_;
  TableModel *delay_sigma_models_[EarlyLate::index_count];
  TableModel *slew_model_;
  TableModel *slew_sigma_models_[EarlyLate::index_count];
  ReceiverModelPtr receiver_model_;
  OutputCurrent *output_current_;
};

class CheckTableModel : public CheckTimingModel
{
public:
  explicit CheckTableModel(TableModel *model,
			   TableModel *sigma_models[EarlyLate::index_count]);
  virtual ~CheckTableModel();
  virtual void checkDelay(const LibertyCell *cell,
			  const Pvt *pvt,
			  float from_slew,
			  float to_slew,
			  float related_out_cap,
			  bool pocv_enabled,
			  // Return values.
			  ArcDelay &margin) const;
  virtual void reportCheckDelay(const LibertyCell *cell,
				const Pvt *pvt,
				float from_slew,
				const char *from_slew_annotation,
				float to_slew,
				float related_out_cap,
				bool pocv_enabled,
				int digits,
				string *result) const;
  const TableModel *model() const { return model_; }

  // Check the axes before making the model.
  // Return true if the model axes are supported.
  static bool checkAxes(const TablePtr table);

protected:
  virtual void setIsScaled(bool is_scaled);
  float findValue(const LibertyLibrary *library,
		  const LibertyCell *cell,
		  const Pvt *pvt,
		  const TableModel *model,
		  float from_slew,
		  float to_slew,
		  float related_out_cap) const;
  void findAxisValues(float from_slew,
		      float to_slew,
		      float related_out_cap,
		      // Return values.
		      float &axis_value1,
		      float &axis_value2,
		      float &axis_value3) const;
  float axisValue(TableAxisPtr axis,
		  float load_cap,
		  float in_slew,
		  float related_out_cap) const;
  void reportTableDelay(const char *result_name,
			const LibertyLibrary *library,
			const LibertyCell *cell,
			const Pvt *pvt,
			const TableModel *model,
			float from_slew,
			const char *from_slew_annotation,
			float to_slew,
			float related_out_cap,
			int digits,
			string *result) const;
  static bool checkAxis(TableAxisPtr axis);

  TableModel *model_;
  TableModel *sigma_models_[EarlyLate::index_count];
};

// Wrapper class for Table to apply scale factors.
class TableModel
{
public:
  TableModel(TablePtr table,
             TableTemplate *tbl_template,
	     ScaleFactorType scale_factor_type,
	     RiseFall *rf);
  void setScaleFactorType(ScaleFactorType type);
  int order() const;
  TableTemplate *tblTemplate() const { return tbl_template_; }
  TableAxisPtr axis1() const;
  TableAxisPtr axis2() const;
  TableAxisPtr axis3() const;
  void setIsScaled(bool is_scaled);
  float value(size_t index1,
              size_t index2,
              size_t index3) const;
  // Table interpolated lookup.
  float findValue(float value1,
		  float value2,
		  float value3) const;
  // Table interpolated lookup with scale factor.
  float findValue(const LibertyLibrary *library,
		  const LibertyCell *cell,
		  const Pvt *pvt,
		  float value1,
		  float value2,
		  float value3) const;
  void reportValue(const char *result_name,
		   const LibertyLibrary *library,
		   const LibertyCell *cell,
		   const Pvt *pvt,
		   float value1,
		   const char *comment1,
		   float value2,
		   float value3,
		   const Unit *table_unit,
                   int digits,
		   string *result) const;
  void report(const Units *units,
	      Report *report) const;

protected:
  float scaleFactor(const LibertyLibrary *library,
		    const LibertyCell *cell,
		    const Pvt *pvt) const;
  void reportPvtScaleFactor(const LibertyLibrary *library,
			    const LibertyCell *cell,
			    const Pvt *pvt,
			    int digits,
			    string *result) const;

  TablePtr table_;
  TableTemplate *tbl_template_;
  // ScaleFactorType gcc barfs if this is dcl'd.
  unsigned scale_factor_type_:scale_factor_bits;
  unsigned tr_index_:RiseFall::index_bit_count;
  bool is_scaled_:1;
};

// Abstract base class for 0, 1, 2, or 3 dimesnion float tables.
class Table
{
public:
  Table() {}
  virtual ~Table() {}
  void setScaleFactorType(ScaleFactorType type);
  virtual int order() const = 0;
  virtual TableAxisPtr axis1() const { return nullptr; }
  virtual TableAxisPtr axis2() const { return nullptr; }
  virtual TableAxisPtr axis3() const { return nullptr; }
  void setIsScaled(bool is_scaled);
  virtual float value(size_t axis_idx1,
                      size_t axis_idx2,
                      size_t axis_idx3) const = 0;
  // Table interpolated lookup.
  virtual float findValue(float value1,
			  float value2,
			  float value3) const = 0;
  // Table interpolated lookup with scale factor.
  float findValue(const LibertyLibrary *library,
		  const LibertyCell *cell,
		  const Pvt *pvt,
		  float value1,
		  float value2,
		  float value3) const;
  virtual void reportValue(const char *result_name,
			   const LibertyLibrary *library,
			   const LibertyCell *cell,
			   const Pvt *pvt,
			   float value1,
			   const char *comment1,
			   float value2,
			   float value3,
                           const Unit *table_unit,
			   int digits,
			   string *result) const = 0;
  virtual void report(const Units *units,
		      Report *report) const = 0;
};

// Zero dimension (scalar) table.
class Table0 : public Table
{
public:
  Table0(float value);
  virtual int order() const { return 0; }
  virtual float value(size_t index1,
                      size_t index2,
                      size_t index3) const;
  virtual float findValue(float value1,
			  float value2,
			  float value3) const;
  virtual void reportValue(const char *result_name,
			   const LibertyLibrary *library,
			   const LibertyCell *cell,
			   const Pvt *pvt,
			   float value1,
			   const char *comment1,
			   float value2,
			   float value3,
                           const Unit *table_unit,
			   int digits,
			   string *result) const;
  virtual void report(const Units *units,
		      Report *report) const;
  using Table::findValue;

private:
  float value_;
};

// One dimensional table.
class Table1 : public Table
{
public:
  Table1(FloatSeq *values,
	 TableAxisPtr axis1);
  virtual ~Table1();
  virtual int order() const { return 1; }
  virtual TableAxisPtr axis1() const { return axis1_; }
  virtual float value(size_t index1,
                      size_t index2,
                      size_t index3) const;
  float value(size_t index1) const;
  virtual float findValue(float value1,
			  float value2,
			  float value3) const;
  virtual void reportValue(const char *result_name,
			   const LibertyLibrary *library,
			   const LibertyCell *cell,
			   const Pvt *pvt,
			   float value1,
			   const char *comment1,
			   float value2,
			   float value3,
                           const Unit *table_unit,
			   int digits,
			   string *result) const;
  virtual void report(const Units *units,
		      Report *report) const;
  using Table::findValue;

private:
  FloatSeq *values_;
  TableAxisPtr axis1_;
};

// Two dimensional table.
class Table2 : public Table
{
public:
  Table2(FloatTable *values,
	 TableAxisPtr axis1,
	 TableAxisPtr axis2);
  virtual ~Table2();
  virtual int order() const { return 2; }
  TableAxisPtr axis1() const { return axis1_; }
  TableAxisPtr axis2() const { return axis2_; }
  virtual float value(size_t index1,
                      size_t index2,
                      size_t index3) const;
  float value(size_t index1,
              size_t index2) const;
  virtual float findValue(float value1,
			  float value2,
			  float value3) const;
  FloatTable *values3() { return values_; }
  virtual void reportValue(const char *result_name,
			   const LibertyLibrary *library,
			   const LibertyCell *cell,
			   const Pvt *pvt,
			   float value1,
			   const char *comment1,
			   float value2,
			   float value3,
                           const Unit *table_unit,
			   int digits,
			   string *result) const;
  virtual void report(const Units *units,
		      Report *report) const;
  using Table::findValue;

protected:
  FloatTable *values_;
  // Row.
  TableAxisPtr axis1_;
  // Column.
  TableAxisPtr axis2_;
};

// Three dimensional table.
class Table3 : public Table2
{
public:
  Table3(FloatTable *values,
	 TableAxisPtr axis1,
	 TableAxisPtr axis2,
	 TableAxisPtr axis3);
  virtual ~Table3() {}
  virtual int order() const { return 3; }
  TableAxisPtr axis3() const { return axis3_; }
  virtual float value(size_t index1,
                      size_t index2,
                      size_t index3) const;
  virtual float findValue(float value1,
			  float value2,
			  float value3) const;
  virtual void reportValue(const char *result_name,
			   const LibertyLibrary *library,
			   const LibertyCell *cell,
			   const Pvt *pvt,
			   float value1,
			   const char *comment1,
			   float value2,
			   float value3,
                           const Unit *table_unit,
			   int digits,
			   string *result) const;
  virtual void report(const Units *units,
		      Report *report) const;
  using Table::findValue;

private:
  TableAxisPtr axis3_;
};

class TableAxis
{
public:
  TableAxis(TableAxisVariable variable,
	    FloatSeq *values);
  ~TableAxis();
  TableAxisVariable variable() const { return variable_; }
  const char *variableString() const;
  const Unit *unit(const Units *units);
  size_t size() const { return values_->size(); }
  float axisValue(size_t index) const { return (*values_)[index]; }
  // Find the index for value such that axis[index] <= value < axis[index+1].
  size_t findAxisIndex(float value) const;
  void findAxisIndex(float value,
                     // Return values.
                     size_t &index,
                     bool &exists) const;
  FloatSeq *values() const { return values_; }

private:
  TableAxisVariable variable_;
  FloatSeq *values_;
};

////////////////////////////////////////////////////////////////

class ReceiverModel
{
public:
  ReceiverModel();
  ~ReceiverModel();
  void setCapacitanceModel(TableModel *table_model,
                           int index,
                           RiseFall *rf);
  static bool checkAxes(TablePtr table);

private:
  TableModel *capacitance_models_[2][RiseFall::index_count];
};

class OutputCurrentWaveform
{
public:
  OutputCurrentWaveform(float axis_value1,
                        float axis_value2,
                        TableAxisPtr axis,
                        Table1 *currents,
                        float reference_time);
  ~OutputCurrentWaveform();
  float axisValue1() const { return axis_value1_; }
  float axisValue2() const { return axis_value2_; }
  TableAxisPtr axis() const { return axis_; }
  Table1 *currents() const { return currents_; }
  float referenceTime() const { return reference_time_; }
  void reportWaveform(const Units *units,
                      int digits,
                      string *result);
  static bool checkAxes(TableTemplate *tbl_template);
  
private:
  float axis_value1_;
  float axis_value2_;
  TableAxisPtr axis_;
  Table1 *currents_;
  float reference_time_;
};

// Two dimensional table of one dimensional time/current tables.
class OutputCurrent
{
public:
  OutputCurrent(TableAxisPtr axis1,
                TableAxisPtr axis2,
                Vector<OutputCurrentWaveform*> &waveforms);
  ~OutputCurrent();
  void reportWaveform(const LibertyCell *cell,
                      const Pvt *pvt,
                      float in_slew,
                      float load_cap,
                      int digits,
                      string *result) const;

private:
  void findAxisValues(float in_slew,
                      float load_cap,
                      // Return values.
                      float &axis_value1,
                      float &axis_value2) const;
  float axisValue(TableAxisPtr axis,
                  float in_slew,
                  float load_cap) const;

  // Row.
  TableAxisPtr axis1_;
  // Column.
  TableAxisPtr axis2_;
  OutputCurrentWaveformSeq waveforms_;
};

} // namespace
