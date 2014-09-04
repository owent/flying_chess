using System;
using System.Collections.Generic;
using System.Collections;
using System.Xml;
using System.Linq;

namespace OverCraft.Data.XML {

	public class XMLNodeListEx : List<XMLNodeEx> {

		public XMLNodeListEx this[string tagName] {
			get {
				var ret = new XMLNodeListEx();
				ret.AddRange(this.TakeWhile(e => tagName == e.Name));
				return ret;
			}
		}

	}
}
