using System;
using System.Collections;
using System.IO;
using System.Xml;

namespace OverCraft.Data.XML {
	public class XMLDocumentEx : XMLNodeEx {

		protected void dealAttributeOfElement(XmlReader rd, XMLNodeEx node) {
			for (var i = 0; i < rd.AttributeCount; i++) {
				rd.MoveToAttribute(i);
				dealAttribute(rd, node);
			}
		}

		protected XMLNodeEx dealElement(XmlReader rd, XMLNodeEx node) {
			var tmp = new XMLNodeEx(rd.LocalName);
			node.ChildNodes.Add(tmp);
			tmp.ParentNode = node;
			dealAttributeOfElement(rd, tmp);
			return tmp;
		}

		protected XMLNodeEx dealAttribute(XmlReader rd, XMLNodeEx node) {
			node.Attributes.Add(rd.Name, rd.Value);
			return node;
		}

		protected XMLNodeEx dealEndElement(XmlReader rd, XMLNodeEx node) {
			return node.ParentNode;
		}

		protected XMLNodeEx dealRead(XmlReader rd, XMLNodeEx node) {
			if (rd.NodeType == XmlNodeType.Element)
				node = dealElement(rd, node);
			else if (rd.NodeType == XmlNodeType.Attribute)
				node = dealAttribute(rd, node);
			else if (rd.NodeType == XmlNodeType.EndElement)
				node = dealEndElement(rd, node);
			return node;
		}

		protected void load(string xml, XMLNodeEx root) {
			var rd = XmlReader.Create(new StringReader(xml));
			var tmp = root;
			var i = 0;
			while (rd.Read()) {
				tmp = dealRead(rd, tmp);
				i++;
			}
			rd.Close();
		}

		protected void setValue(XMLNodeEx target) {
			this.ParentNode = null;
			this.Name = target.Name;
			this.ChildNodes = target.ChildNodes;
			this.Attributes = target.Attributes;
		}

		public bool Load(string xml) {
			var root = new XMLNodeEx();
			load(xml, root);
			if (root.ChildNodes.Count > 0) {
				setValue(root.ChildNodes[0]);
				return true;
			}
			return false;
		}

	}
}
