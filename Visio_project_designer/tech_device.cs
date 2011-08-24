﻿/// @file tech_device.cs
/// @brief Классы, которые реализуют функциональность для работы простыми 
/// устройствами (клапана, насосы и т.д.).
/// 
/// @author  Иванюк Дмитрий Сергеевич.
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.
/// 

using System;
using System.Collections.Generic;

using System.Text.RegularExpressions;
using System.Windows.Forms;

using Visio = Microsoft.Office.Interop.Visio;

using wago;

/// <summary> Устройства проекта (клапан, насос, танк...).</summary>
namespace tech_device
    {
    /// <summary> Простое устройство (клапан, насос, ...). </summary>
    ///
    /// <remarks> Id, 17.08.2011. </remarks>
    public class device : wago_device
        {

        /// <summary> Номер устройства.  </summary>
        private int n;

        /// <summary> Получение номера устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <returns> Номер устройства. </returns>
        public int get_n()
            {
            return n;
            }

        /// <summary> Типы устройств. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public enum TYPES
            {
            /// <summary> Клапан.  </summary>
            T_V = 0, 
            }

        /// <summary> Подтип клапана. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public enum SUB_TYPES
            {
            /// <summary> 1 канал управления.  </summary>
            V_1_CONTROL_CHANNEL = 0,

            /// <summary> 2 канала управления.  </summary>
            V_2_CONTROL_CHANNEL,


            /// <summary> 1 канал управления и 1 обратная связь.  </summary>
            V_1_CONTROL_CHANNEL_1_FB,

            /// <summary> 1 канал управления и 2 обратных связи.  </summary>
            V_1_CONTROL_CHANNEL_2_FB,

            /// <summary> 2 канала управления и 2 обратных связи.  </summary>
            V_2_CONTROL_CHANNEL_2_FB,
            }
        
        /// <summary> Тип устройства. </summary>
        TYPES type;

        /// <summary> Подтип устройства. </summary>
        SUB_TYPES sub_type;

        /// <summary> Активный (выбранный) в данный момент канал устройства (для визуальной
        ///  работы с устройством). </summary>
        private string active_channel_name = "";

        /// <summary> Получение активного канала. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <returns> Активный канал. </returns>
        public string get_active_channel()
            {
            return active_channel_name;
            }

        /// <summary> Обновление окна со свойствами устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="cbox">           The cbox. </param>
        /// <param name="lview">          The lview control. </param>
        /// <param name="only_list_view"> (optional) the only list view. </param>
        public void refresh_edit_window( ComboBox cbox, ListView lview, 
            bool only_list_view = false )
            {
            if( only_list_view == false )
                {
                cbox.Items.Clear();
                cbox.Items.AddRange( SUB_NAMES );

                cbox.SelectedIndex = ( int ) sub_type;
                }

            lview.Items.Clear();

            Dictionary<string, string> items_str = to_str();
            foreach( KeyValuePair<string, string> item_str in items_str )
                {
                System.Windows.Forms.ListViewItem item;
                item = lview.Items.Add( item_str.Key );
                item.SubItems.Add( item_str.Value );

                if( active_channel_name == item_str.Key )
                    {
                    item.Selected = true;
                    lview.Focus();
                    }
                }
            }

        /// <summary> Представление в виде списка строк, которые описывают каналы
        /// устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <returns> A string representation of this object. </returns>
        private Dictionary<string, string> to_str()
            {
            Dictionary<string, string> res = new Dictionary<string, string>();

            foreach( KeyValuePair<string, wago_channel> channel in wago_channels )
                {
                string description = "не привязан";
                if( channel.Value.module != null )
                    {
                    description = string.Format( "узел {0} модуль {1} клемма {2}",
                        channel.Value.module.node_number,
                        channel.Value.module.order_number, channel.Value.clamp + 1 );
                    }

                res.Add( channel.Key, description );
                }

            return res;
            }

        /// <summary> Получение типа устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <returns> The type. </returns>
        public int get_type()
            {
            return ( int ) type;
            }

        /// <summary> Получение подтипа устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <returns> The sub type. </returns>
        public int get_sub_type()
            {
            return ( int ) sub_type;
            }
        
        /// <summary> Фигура Visio устройства. </summary>
        private Visio.Shape shape;
        
        /// <summary> Имена типов устройств.  </summary>
        private string[] NAMES = 
        {
        "Клапан",
        };
        
        /// <summary> Имена подтипов устройств.  </summary>
        private string[] SUB_NAMES = 
        {
        "1 КУ",
        "2 КУ",
        "1 КУ 1 ОС",
        "1 КУ 2 ОС",
        "2 КУ 2 ОС"
        };

        /// <summary> Получение параметров канала из строкового описания канала
        /// (для записи\чтения из файла). </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="str">    The string. </param>
        /// <param name="node">   [out] The node. </param>
        /// <param name="module"> [out] The module. </param>
        /// <param name="clamp">  [out] The clamp. </param>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        private bool get_n_from_str( string str, out int node, out int module,
            out int clamp )
            {
            //Значение привязки имеет следующий формат:
            // -> узел 1 модуль 2 клемма 3;
            //на его основе получаем индекс подсвечиваемого объекта 
            //(номер узла и модуля) и номер клеммы:
            // -> 1, 2, 3.
            node = 0;
            module = 0;
            clamp = 0;

            Regex rex = new Regex( @"\w+\s(\d+)\s\w+\s(\d+)\s\w+\s(\d+)",
                RegexOptions.IgnoreCase );
            if( rex.IsMatch( str ) )
                {
                Match mtc = rex.Match( str );

                node = Convert.ToInt16( mtc.Groups[ 1 ].ToString() ) - 1;
                module = Convert.ToInt16( mtc.Groups[ 2 ].ToString() ) - 1;
                clamp = Convert.ToInt16( mtc.Groups[ 3 ].ToString() ) - 1;
                return true;
                }

            return false;
            }

        /// <summary> Изменение подтипа устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="sub_type"> Подтип устройства. </param>
        /// <param name="pac">      Узел Wago. </param>
        public void change_sub_type( SUB_TYPES sub_type, PAC pac )
            {
            this.sub_type = sub_type;
            shape.Cells[ "Prop.sub_type" ].FormulaU = Convert.ToString( ( int ) sub_type );

            set_sub_type( sub_type, pac );
            }

        /// <summary> Установка подтипа устройства. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="sub_type"> Подтип устройства. </param>
        /// <param name="pac">      Узел Wago. </param>
        private void set_sub_type( SUB_TYPES sub_type, PAC pac )
            {
            wago_channels.Clear();

            switch( type )
                {
                case TYPES.T_V:

                    string str_DO1 = shape.Cells[ "Prop.DO1" ].Formula;
                    string str_DO2 = shape.Cells[ "Prop.DO2" ].Formula;
                    string str_DI1 = shape.Cells[ "Prop.DI1" ].Formula;
                    string str_DI2 = shape.Cells[ "Prop.DI2" ].Formula;

                    int node;
                    int module;
                    int clamp;

                    switch( sub_type )
                        {
                        case SUB_TYPES.V_1_CONTROL_CHANNEL:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, module, clamp );

                            break;

                        case SUB_TYPES.V_2_CONTROL_CHANNEL:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            add_wago_channel( "DO2", io_module.KINDS.DO );

                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, module, clamp );

                            get_n_from_str( str_DO2, out node, out module, out clamp );
                            wago_channels[ "DO2" ].set( pac, module, clamp );

                            break;

                        case SUB_TYPES.V_1_CONTROL_CHANNEL_1_FB:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            add_wago_channel( "DI1", io_module.KINDS.DI );

                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, module, clamp );

                            get_n_from_str( str_DI1, out node, out module, out clamp );
                            wago_channels[ "DI1" ].set( pac, module, clamp );
                            break;

                        case SUB_TYPES.V_1_CONTROL_CHANNEL_2_FB:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            add_wago_channel( "DI1", io_module.KINDS.DI );
                            add_wago_channel( "DI2", io_module.KINDS.DI );

                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, module, clamp );

                            get_n_from_str( str_DI1, out node, out module, out clamp );
                            wago_channels[ "DI1" ].set( pac, module, clamp );

                            get_n_from_str( str_DI2, out node, out module, out clamp );
                            wago_channels[ "DI2" ].set( pac, module, clamp );

                            break;

                        case SUB_TYPES.V_2_CONTROL_CHANNEL_2_FB:
                            add_wago_channel( "DO1", io_module.KINDS.DO );
                            add_wago_channel( "DO2", io_module.KINDS.DO );
                            add_wago_channel( "DI1", io_module.KINDS.DI );
                            add_wago_channel( "DI2", io_module.KINDS.DI );

                            get_n_from_str( str_DO1, out node, out module, out clamp );
                            wago_channels[ "DO1" ].set( pac, module, clamp );

                            get_n_from_str( str_DO2, out node, out module, out clamp );
                            wago_channels[ "DO2" ].set( pac, module, clamp );

                            get_n_from_str( str_DI1, out node, out module, out clamp );
                            wago_channels[ "DI1" ].set( pac, module, clamp );

                            get_n_from_str( str_DI2, out node, out module, out clamp );
                            wago_channels[ "DI2" ].set( pac, module, clamp );

                            break;
                        }
                    break;
                }

            }

        /// <summary> Constructor. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="shape"> Фигура Visio. </param>
        /// <param name="pac">   Узел Wago. </param>
        public device( Visio.Shape shape, PAC pac )
            {
            n = Convert.ToUInt16( shape.Cells[ "Prop.number" ].FormulaU );
            type = ( TYPES ) Convert.ToUInt16( shape.Cells[ "Prop.type" ].FormulaU );
            sub_type = ( SUB_TYPES ) Convert.ToUInt16( shape.Cells[ "Prop.sub_type" ].FormulaU );

            this.shape = shape;

            set_sub_type( sub_type, pac );
            }

        /// <summary> Привязка канала к клемме. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="channel_name"> Name of the channel. </param>
        /// <param name="module">       module. </param>
        /// <param name="clamp">        The clamp. </param>
        public void set_channel( string channel_name, io_module module, int clamp )
            {
            //Освобождаем при наличии ранее привязанную клемму.

            if( wago_channels[ channel_name ].module != null )
                {
                wago_channels[ channel_name ].module.free(
                    wago_channels[ channel_name ].clamp );
                }
            wago_channels[ channel_name ].module = module;
            wago_channels[ channel_name ].clamp = clamp;

            string prop = "Prop." + channel_name;
            string value = string.Format(
                "\"узел {0} модуль {1} клемма {2}\"",
                module.node_number, module.order_number, clamp + 1 );

            shape.Cells[ prop ].FormulaU = value;


            module.use( clamp );
            }

        /// <summary> Установка активной ("подсвечиваемой") клеммы в данный 
        /// момент времени. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        ///
        /// <param name="channel_name"> Name of the channel. </param>
        public void set_active_channel( string channel_name )
            {
            active_channel_name = channel_name;
            }

        /// <summary> Отмена "подсветки" клемм, занятых устройством. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public void unselect_channels()
            {
            foreach( KeyValuePair<string, wago_channel> channel in wago_channels )
                {
                if( channel.Value.module != null )
                    {
                    channel.Value.module.deactivate();
                    }
                }
            }

        /// <summary> "Подсветка" клемм, занятых устройством. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public void select_channels()
            {
            foreach( KeyValuePair<string, wago_channel> channel in wago_channels )
                {
                if( channel.Value.module != null )
                    {
                    channel.Value.module.activate( channel.Value.clamp );
                    }
                }

            if( active_channel_name != "" &&
                wago_channels.ContainsKey( active_channel_name ) &&
                wago_channels[ active_channel_name ].module != null )
                {
                wago_channels[ active_channel_name ].module.magnify_clamp(
                    wago_channels[ active_channel_name ].clamp );
                }
            }

        }
    }