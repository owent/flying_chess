using System;
using System.Collections;
using System.Collections.Generic;
using System.Xml;

namespace OverCraft.Data.XML {

	public class XMLNodeEx {
		public string Name;
		public Dictionary<string, string> Attributes = new Dictionary<string, string>();
		public XMLNodeEx ParentNode;
		public XMLNodeListEx ChildNodes = new XMLNodeListEx();

		public XMLNodeEx(string tagName = "") {
			Name = tagName;
		}

		public XMLNodeEx GetFirstChild(string tagname) {
			var res = null as XMLNodeEx;
			foreach (var c in ChildNodes) {
				if (c.Name == tagname)
					return c;
				if ((res = c.GetFirstChild(tagname)) != null)
					return res;
			}
			return res;
		}
	}
}
